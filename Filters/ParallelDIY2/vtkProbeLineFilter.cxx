/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProbeLineFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkProbeLineFilter.h"

#include "vtkAggregateDataSetFilter.h"
#include "vtkAppendArcLength.h"
#include "vtkAppendDataSets.h"
#include "vtkCellCenters.h"
#include "vtkCompositeDataSet.h"
#include "vtkCutter.h"
#include "vtkDIYExplicitAssigner.h"
#include "vtkDIYUtilities.h"
#include "vtkDataArrayRange.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkMathUtilities.h"
#include "vtkMultiProcessController.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPProbeFilter.h"
#include "vtkPlane.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyLineSource.h"
#include "vtkSMPTools.h"
#include "vtkSmartPointer.h"
#include "vtkStaticCellLocator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkStripper.h"
#include "vtkVectorOperators.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

// clang-format off
#include "vtk_diy2.h"
#include VTK_DIY2(diy/master.hpp)
#include VTK_DIY2(diy/mpi.hpp)
// clang-format off

vtkStandardNewMacro(vtkProbeLineFilter);

vtkCxxSetObjectMacro(vtkProbeLineFilter, Controller, vtkMultiProcessController);

namespace
{
//==============================================================================
struct PointSetBlock
{
  std::vector<std::vector<double>> ReceivedProjections;
};

//==============================================================================
// This worker generates the line profile given an input of sorted points along the line.
// Its behavior depends on SamplingPattern: it duplicates the points, moving them slightly
// in each direction of the line, in the instance of `SAMPLE_LINE_AT_CELL_BOUNDARIES`.
struct ProbingPointGeneratorWorker
{
  ProbingPointGeneratorWorker(
    const std::vector<double>& projections, const double* point1, const double* point2, double tolerance, bool splitPoints)
    : Projections(projections)
    , Point1(point1)
    , Point2(point2)
    , SplitPoints(splitPoints)
    , LineDirection((vtkVector3d(point2) - vtkVector3d(point1)))
    , LineNorm(this->LineDirection.Normalize())
    , LineDirectionEpsilon(2.0 * tolerance * LineDirection)
  {
    // We add 2 points to add Point1 and Point2 to the list of probed points when the for loop is done
    auto size = projections.size();
    if (splitPoints)
    {
      this->SortedPoints->SetNumberOfPoints(2 * size + 2);
    }
    else
    {
      this->SortedPoints->SetNumberOfPoints(size + 2);
    }
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    double point1Epsilon = VTK_DBL_EPSILON *
      std::max({ std::abs(this->Point1[0]), std::abs(this->Point1[1]), std::abs(this->Point1[2]) });
    double point2Epsilon = VTK_DBL_EPSILON *
      std::max({ std::abs(this->Point2[0]), std::abs(this->Point2[1]), std::abs(this->Point2[2]) });
    vtkVector3d p1v(this->Point1);

    for (vtkIdType pointId = startId; pointId < endId; ++pointId)
    {
      vtkVector3d p = p1v + this->LineDirection * (this->Projections[pointId] / this->LineNorm);
      if (this->SplitPoints)
      {
        vtkIdType idx = pointId * 2 + 1;
        // We make sure that we do not add any point before Point1 or after Point2.
        // If this happens, we replace the point by Point1 or Point2 when appropriate.
        // In such instances, there will unnecessary duplicate points in the probing line.
        // It is not a problem for the rest of the filter, and keeping consistently
        // the rule that the first and last points are the end points of the input line
        // makes it more trivial to get rid of them if they are not required later
        // in the pipeline (getting rid of the 2 first samples and the 2 last samples
        // is sufficient).
        double pBefore[3], pAfter[3], tmp[3];

        vtkMath::Subtract(p.GetData(), this->LineDirectionEpsilon.GetData(), pBefore);
        vtkMath::Subtract(pBefore, this->Point1, tmp);
        if (vtkMath::Dot(this->LineDirection.GetData(), tmp) < point1Epsilon)
        {
          this->SortedPoints->SetPoint(idx, this->Point1);
        }
        else
        {
          this->SortedPoints->SetPoint(idx, pBefore);
        }

        vtkMath::Add(p.GetData(), this->LineDirectionEpsilon.GetData(), pAfter);
        vtkMath::Subtract(this->Point2, pAfter, tmp);
        if (vtkMath::Dot(this->LineDirection.GetData(), tmp) < point2Epsilon)
        {
          this->SortedPoints->SetPoint(idx + 1, this->Point2);
        }
        else
        {
          this->SortedPoints->SetPoint(idx + 1, pAfter);
        }
      }
      else
      {
        this->SortedPoints->SetPoint(pointId + 1, p.GetData());
      }
    }
  }

  const std::vector<double>& Projections;
  const double* Point1;
  const double* Point2;
  bool SplitPoints;
  vtkVector3d LineDirection;
  double LineNorm;
  vtkVector3d LineDirectionEpsilon;

  // output
  vtkNew<vtkPoints> SortedPoints;
};
}

//------------------------------------------------------------------------------
vtkProbeLineFilter::vtkProbeLineFilter()
  : Controller(nullptr)
  , SamplingPattern(SAMPLE_LINE_AT_CELL_BOUNDARIES)
  , LineResolution(1000)
  , Point1{ -0.5, 0.0, 0.0 }
  , Point2{ 0.5, 0.0, 0.0 }
  , ComputeTolerance(true)
  , Tolerance(1.0)
{
  this->SetNumberOfInputPorts(1);
  this->SetController(vtkMultiProcessController::GetGlobalController());
}

//------------------------------------------------------------------------------
vtkProbeLineFilter::~vtkProbeLineFilter()
{
  this->SetController(nullptr);
}

//------------------------------------------------------------------------------
int vtkProbeLineFilter::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());

  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkDataSet* output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!outInfo || !inInfo)
  {
    vtkErrorMacro("No input or output information");
  }

  vtkSmartPointer<vtkPolyData> sampledLine;

  switch (this->SamplingPattern)
  {
    case SAMPLE_LINE_AT_CELL_BOUNDARIES:
    case SAMPLE_LINE_AT_SEGMENT_CENTERS:
      sampledLine = this->SampleLineAtEachCell(vtkCompositeDataSet::GetDataSets(input));
      break;
    case SAMPLE_LINE_UNIFORMLY:
      sampledLine = this->SampleLineUniformly();
      break;
    default:
      vtkErrorMacro("Sampling heuristic wrongly set... Aborting");
      return 0;
  }

  vtkNew<vtkPProbeFilter> prober;
  prober->SetController(this->Controller);
  prober->SetPassPartialArrays(this->PassPartialArrays);
  prober->SetPassCellArrays(this->PassCellArrays);
  prober->SetPassPointArrays(this->PassPointArrays);
  prober->SetPassFieldArrays(this->PassFieldArrays);
  prober->SetComputeTolerance(false);
  prober->SetTolerance(0.0);
  prober->SetSourceData(input);
  prober->SetInputData(sampledLine);
  prober->Update();

  if (this->Controller->GetLocalProcessId() == 0 &&
    this->SamplingPattern == SAMPLE_LINE_AT_CELL_BOUNDARIES)
  {
    // We move points to the cell interfaces.
    // They were artificially moved away from the cell interfaces so probing works well.
    vtkPointSet* points = vtkPointSet::SafeDownCast(prober->GetOutputDataObject(0));
    auto pointsRange = vtk::DataArrayTupleRange<3>(points->GetPoints()->GetData());
    using PointRef = decltype(pointsRange)::TupleReferenceType;
    for (vtkIdType pointId = 1; pointId < pointsRange.size() - 1; pointId += 2)
    {
      PointRef p1 = pointsRange[pointId];
      PointRef p2 = pointsRange[pointId + 1];

      p1[0] = p2[0] = 0.5 * (p1[0] + p2[0]);
      p1[1] = p2[1] = 0.5 * (p1[1] + p2[1]);
      p1[2] = p2[2] = 0.5 * (p1[2] + p2[2]);
    }
  }

  vtkNew<vtkAppendArcLength> arcs;
  arcs->SetInputConnection(prober->GetOutputPort());
  arcs->Update();

  output->ShallowCopy(arcs->GetOutputDataObject(0));

  return 1;
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> vtkProbeLineFilter::SampleLineUniformly() const
{
  vtkNew<vtkLineSource> lineSource;
  lineSource->SetPoint1(this->Point1);
  lineSource->SetPoint2(this->Point2);
  lineSource->SetResolution(this->LineResolution);
  lineSource->Update();
  return vtkPolyData::SafeDownCast(lineSource->GetOutputDataObject(0));
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> vtkProbeLineFilter::SampleLineAtEachCell(
  const std::vector<vtkDataSet*>& inputs) const
{
  if (vtkMathUtilities::NearlyEqual(this->Point1[0], this->Point2[0]) &&
    vtkMathUtilities::NearlyEqual(this->Point1[1], this->Point2[1]) &&
    vtkMathUtilities::NearlyEqual(this->Point1[2], this->Point2[2]))
  {
    // In this instance, we probe only Point1 and Point2.
    vtkNew<vtkLineSource> line;
    line->SetPoint1(this->Point1);
    line->SetPoint2(this->Point2);
    line->Update();
    return vtkPolyData::SafeDownCast(line->GetOutputDataObject(0));
  }

  vtkVector3d p1v = vtkVector3d(this->Point1);
  vtkVector3d p2v = vtkVector3d(this->Point2);
  const vtkVector3d p12 = p2v - p1v;
  vtkVector3d np12 = p12.Normalized();
  std::vector<double> projections;
  for (std::size_t dsId = 0; dsId < inputs.size(); ++dsId)
  {
    vtkDataSet* input = inputs[dsId];
    const double tolerance = this->ComputeTolerance
      ? VTK_TOL * p12.Norm()
      : this->Tolerance;
    vtkNew<vtkStaticCellLocator> locator;
    locator->SetDataSet(input);
    locator->SetTolerance(tolerance);
    locator->UseDiagonalLengthToleranceOff();
    locator->BuildLocator();

    double t;
    double x[3], val[3], npos[3], _opt[3];
    vtkIdType _id;
    int subId;
    double p1[3] = {this->Point1[0], this->Point1[1], this->Point1[2]};
    double p2[3] = {this->Point2[0], this->Point2[1], this->Point2[2]};
    // if P1 or P2 intersect, skip them and shift the starting/ending points towards the inside
    // of the line as they will be added later anyway and we don't want to duplicate them
    if (locator->FindCell(p1) >= 0)
    {
      p1[0] += 2.0 * tolerance * np12.GetX();
      p1[1] += 2.0 * tolerance * np12.GetY();
      p1[2] += 2.0 * tolerance * np12.GetZ();
    }
    if (locator->FindCell(p2) >= 0)
    {
      p2[0] -= 2.0 * tolerance * np12.GetX();
      p2[1] -= 2.0 * tolerance * np12.GetY();
      p2[2] -= 2.0 * tolerance * np12.GetZ();
    }
    vtkNew<vtkGenericCell> cell;
    while (locator->IntersectWithLine(p1, p2, 0.0, t, x, _opt, subId, _id, cell))
    {
      if (cell->GetCellDimension() == 3)
      {
        if (this->SamplingPattern == SamplingPattern::SAMPLE_LINE_AT_SEGMENT_CENTERS)
        {
          double pcoords[3];
          cell->GetParametricCenter(pcoords);
          cell->EvaluateLocation(subId, pcoords, val, _opt);
          memcpy(x, val, 3 * sizeof(double));
        }

        for (vtkIdType f = 0; f < cell->GetNumberOfFaces(); ++f)
        {
          vtkCell* face = cell->GetFace(f);
          if (face->IntersectWithLine(x, p2, 0.0, t, npos, _opt, subId))
          {
            if (this->SamplingPattern == SamplingPattern::SAMPLE_LINE_AT_CELL_BOUNDARIES)
            {
              memcpy(val, npos, 3 * sizeof(double));
            }
            break;
          }
        }
      }
      else
      {
        memcpy(val, x, 3 * sizeof(double));
        memcpy(npos, x, 3 * sizeof(double));
      }

      // Add current intersection/center point projection. We don't use the coordinates directly because
      // the projection will be easier to sort and tranfer through processes. Also one could directly add
      // the point distance from P1 but it's more numerically stable to add its projection onto the vector p12.
      projections.emplace_back(p12.Dot(vtkVector3d(val) - p1v));

      // Pad by epsilon to not intersect the same cell twice
      p1[0] = npos[0] + 2.0 * tolerance * np12.GetX();
      p1[1] = npos[1] + 2.0 * tolerance * np12.GetY();
      p1[2] = npos[2] + 2.0 * tolerance * np12.GetZ();

      // Check if we're done ie we have passed P2. This is needed when P2 is at the border,
      // in this case the locator will always intersect so we have to break ourself.
      if (p12.Dot(p2v - vtkVector3d(p1)) < 0)
      {
        break;
      }
    }
  }

  // Sort our array of projections so the merge across ranks is faster afterward.
  vtkSMPTools::Sort(projections.begin(), projections.end());

  // We need to gather points from every ranks to every ranks because vtkProbeFilter
  // assumes that its input is replicated in every ranks.
  diy::mpi::communicator comm = vtkDIYUtilities::GetCommunicator(this->Controller);
  diy::Master master(
    comm, 1, -1, []() { return static_cast<void*>(new PointSetBlock()); },
    [](void* b) -> void { delete static_cast<PointSetBlock*>(b); });
  vtkDIYExplicitAssigner assigner(comm, 1);
  diy::RegularDecomposer<diy::DiscreteBounds> decomposer(
    /*dim*/ 1, diy::interval(0, assigner.nblocks() - 1), assigner.nblocks());
  decomposer.decompose(comm.rank(), assigner, master);

  diy::all_to_all(
    master, assigner, [&master, &projections](PointSetBlock* block, const diy::ReduceProxy& srp) {
      int myBlockId = srp.gid();
      if (srp.round() == 0)
      {
        for (int i = 0; i < srp.out_link().size(); ++i)
        {
          const diy::BlockID& blockId = srp.out_link().target(i);
          if (blockId.gid != myBlockId)
          {
            srp.enqueue(blockId, projections);
          }
        }
      }
      else
      {
        for (int i = 0; i < static_cast<int>(srp.in_link().size()); ++i)
        {
          const diy::BlockID& blockId = srp.in_link().target(i);
          if (blockId.gid != myBlockId)
          {
            std::vector<double> data;
            srp.dequeue(blockId, data);
            block->ReceivedProjections.emplace_back(std::move(data));
          }
        }
      }
    });

  // Merge local projections with projections from all other ranks
  PointSetBlock* block = master.block<PointSetBlock>(0);
  for (const auto& distProjections : block->ReceivedProjections)
  {
    auto prevEnd = projections.insert(projections.end(), distProjections.begin(), distProjections.end());
    std::inplace_merge(projections.begin(), prevEnd, projections.end());
  }

  // Duplicate points can happen on composite data set so lets remove them
  auto last = std::unique(projections.begin(), projections.end(), [](double l, double r)
    {
      return vtkMathUtilities::NearlyEqual(l, r);
    });
  projections.erase(last, projections.end());

  bool splitPoints = this->SamplingPattern == SAMPLE_LINE_AT_CELL_BOUNDARIES;
  double splitDelta = this->ComputeTolerance ? VTK_TOL : this->Tolerance;
  ProbingPointGeneratorWorker probingPointGeneratorWorker(projections,
    this->Point1, this->Point2, splitDelta, splitPoints);
  vtkSMPTools::For(0, projections.size(), probingPointGeneratorWorker);

  vtkPoints* sortedPoints = probingPointGeneratorWorker.SortedPoints;
  sortedPoints->SetPoint(0, this->Point1);
  sortedPoints->SetPoint(sortedPoints->GetNumberOfPoints() - 1, this->Point2);

  vtkNew<vtkPolyLineSource> polyLine;
  polyLine->SetPoints(sortedPoints);
  polyLine->Update();

  return vtkPolyData::SafeDownCast(polyLine->GetOutputDataObject(0));
}

//------------------------------------------------------------------------------
int vtkProbeLineFilter::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkCompositeDataSet");
  return 1;
}

//------------------------------------------------------------------------------
void vtkProbeLineFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Controller: " << this->Controller << endl;
  switch (this->SamplingPattern)
  {
    case SAMPLE_LINE_AT_CELL_BOUNDARIES:
      os << indent << "SamplingPattern: SAMPLE_LINE_AT_CELL_BOUNDARIES" << endl;
      break;
    case SAMPLE_LINE_AT_SEGMENT_CENTERS:
      os << indent << "SamplingPattern: SAMPLE_LINE_AT_SEGMENT_CENTERS" << endl;
      break;
    case SAMPLE_LINE_UNIFORMLY:
      os << indent << "SamplingPattern: SAMPLE_LINE_UNIFORMLY" << endl;
      break;
    default:
      os << indent << "SamplingPattern: UNDEFINED" << endl;
      break;
  }
  os << indent << "LineResolution: " << this->LineResolution << endl;
  os << indent << "PassPartialArrays: " << this->PassPartialArrays << endl;
  os << indent << "PassCellArrays: " << this->PassCellArrays << endl;
  os << indent << "PassPointArrays: " << this->PassPointArrays << endl;
  os << indent << "PassFieldArrays: " << this->PassFieldArrays << endl;
  os << indent << "ComputeTolerance: " << this->ComputeTolerance << endl;
  os << indent << "Tolerance: " << this->Tolerance << endl;
  os << indent << "Point1 = [" << this->Point1[0] << ", " << this->Point1[1] << ", "
     << this->Point1[2] << "]" << endl;
  os << indent << "Point2 = [" << this->Point2[0] << ", " << this->Point2[1] << ", "
     << this->Point2[2] << "]" << endl;
}
