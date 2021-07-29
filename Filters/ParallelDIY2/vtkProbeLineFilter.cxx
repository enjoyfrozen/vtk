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
struct ProjInfo
{
  double Projection;
  vtkIdType CellID;

  static bool SortFunction(const ProjInfo& l, const ProjInfo& r)
  {
    return l.Projection < r.Projection;
  }
  static bool EqualFunction(const ProjInfo& l, const ProjInfo& r)
  {
    return l.CellID == r.CellID && vtkMathUtilities::NearlyEqual(l.Projection, r.Projection);
  }
};

//==============================================================================
struct OptionalPosition
{
  bool IsValid;
  vtkVector3d Position;
};

std::pair<OptionalPosition,OptionalPosition> GetInOutCell3D(const vtkVector3d& p1, const vtkVector3d& p2, vtkCell* cell, double tolerance)
{
  using PosAndDist = std::pair<vtkVector3d,double>;
  const int nface = cell->GetNumberOfFaces();
  std::vector<PosAndDist> intersections;
  intersections.reserve(nface);
  double t, x[3], _pcoords[3];
  int _id;
  for (int i = 0; i < nface; ++i)
  {
    if (cell->GetFace(i)->IntersectWithLine(p1.GetData(), p2.GetData(), 0.0, t, x, _pcoords, _id))
    {
      intersections.emplace_back(PosAndDist(vtkVector3d(x), t));
    }
  }
  std::sort(intersections.begin(), intersections.end(), [](const PosAndDist& l, const PosAndDist& r)
    {
      return l.second < r.second;
    });
  auto last = std::unique(intersections.begin(), intersections.end(), [tolerance](const PosAndDist& l, const PosAndDist& r)
    {
      return vtkMathUtilities::NearlyEqual(l.second, r.second, tolerance);
    });

  const std::size_t size = std::distance(intersections.begin(), last);
  OptionalPosition inProj{false, vtkVector3d()};
  OptionalPosition outProj{false, vtkVector3d()};
  if (size == 1)
  {
    outProj = {true, intersections[0].first};
  }
  else if (size >= 2)
  {
    inProj = {true, intersections[0].first};
    outProj = {true, (last - 1)->first};
  }

  return std::pair<OptionalPosition,OptionalPosition>(inProj, outProj);
}

//==============================================================================
std::vector<ProjInfo> ProcessLimitPoints(vtkVector3d p1, vtkVector3d p2, int pattern, vtkDataSet* input, vtkStaticCellLocator* locator, double tolerance)
{
  double t, x[3], pcoords[3], _tmp[3], bb[6];
  int subId;
  std::vector<ProjInfo> projections;
  vtkVector3d nline = (p2 - p1).Normalized();

  vtkIdType p1Cell = locator->FindCell(p1.GetData());
  if (p1Cell >= 0)
  {
    vtkCell* cell = input->GetCell(p1Cell);
    cell->GetBounds(bb);
    if (cell->GetCellDimension() == 3)
    {
      auto pair = ::GetInOutCell3D(p1, p2, cell, tolerance);
      OptionalPosition outPos = pair.second;
      if (outPos.IsValid)
      {
        if (pattern == vtkProbeLineFilter::SamplingPattern::SAMPLE_LINE_AT_SEGMENT_CENTERS)
        {
          double projection = nline.Dot(outPos.Position - p1);
          // Equivalent to (projection + P1Proj) * 0.5, with P1Proj = nline.Dot(p1 - p1) = 0
          projections.emplace_back(ProjInfo{projection * 0.5, p1Cell});
        }
        else
        {
          double projection = nline.Dot((outPos.Position - p1) - tolerance * nline);
          projections.emplace_back(ProjInfo{projection, p1Cell});
        }
      }
    }
  }

  vtkIdType p2Cell = locator->FindCell(p2.GetData());
  if (p2Cell >= 0)
  {
    vtkCell* cell = input->GetCell(p2Cell);
    cell->GetBounds(bb);
    if (cell->GetCellDimension() == 3)
    {
      auto pair = ::GetInOutCell3D(p1, p2, cell, tolerance);
      OptionalPosition inPos = pair.second;
      if (pair.first.IsValid)
      {
        inPos = pair.first;
      }
      if (inPos.IsValid)
      {
        if (pattern == vtkProbeLineFilter::SamplingPattern::SAMPLE_LINE_AT_SEGMENT_CENTERS)
        {
          double projection = nline.Dot((inPos.Position + p2) * 0.5 - p1);
          projections.emplace_back(ProjInfo{projection, p2Cell});
        }
        else
        {
          double projection = nline.Dot((inPos.Position - p1) + tolerance * nline);
          projections.emplace_back(ProjInfo{projection, p2Cell});
        }
      }
    }
  }

  return projections;
}

//==============================================================================
struct PointSetBlock
{
  std::vector<std::vector<ProjInfo>> ReceivedProjections;
};

//==============================================================================
// This worker generates the line profile given an input of sorted points along the line.
// Its behavior depends on SamplingPattern: it duplicates the points, moving them slightly
// in each direction of the line, in the instance of `SAMPLE_LINE_AT_CELL_BOUNDARIES`.
struct ProbingPointGeneratorWorker
{
  ProbingPointGeneratorWorker(
    const std::vector<ProjInfo>& projections, vtkVector3d point1, vtkVector3d point2)
    : Projections(projections)
    , Start(point1)
    , LineDirection((point2 - point1).Normalized())
  {
    // We add 2 points to add Point1 and Point2 to the list of probed points when the for loop is done
    this->SortedPoints->SetNumberOfPoints(projections.size() + 2);
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    for (vtkIdType pointId = startId; pointId < endId; ++pointId)
    {
      vtkVector3d p = this->Start + this->LineDirection * this->Projections[pointId].Projection;
      this->SortedPoints->SetPoint(pointId + 1, p.GetData());
    }
  }

  const std::vector<ProjInfo>& Projections;
  vtkVector3d Start;
  vtkVector3d LineDirection;

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

  vtkVector3d p1 = vtkVector3d(this->Point1);
  vtkVector3d p2 = vtkVector3d(this->Point2);
  const vtkVector3d line = p2 - p1;
  const vtkVector3d nline = line.Normalized();
  std::vector<ProjInfo> projections;
  const double tolerance = this->ComputeTolerance
    ? VTK_TOL * line.Norm()
    : this->Tolerance;

  // Add every intersections with all blocks of the dataset on our current rank
  for (std::size_t dsId = 0; dsId < inputs.size(); ++dsId)
  {
    vtkDataSet* input = inputs[dsId];
    vtkNew<vtkStaticCellLocator> locator;
    locator->SetDataSet(input);
    locator->SetTolerance(0.0);
    locator->UseDiagonalLengthToleranceOff();
    locator->BuildLocator();

    vtkNew<vtkIdList> intersected;
    locator->FindCellsAlongLine(p1.GetData(), p2.GetData(), 0.0, intersected);
    // We process P1 and P2 a bit differently so in the case of their intersection with a cell
    // they are not duplicated
    auto processedProj = ::ProcessLimitPoints(p1, p2, this->SamplingPattern, input, locator, tolerance);
    for (const auto& proj : processedProj)
    {
      intersected->DeleteId(proj.CellID);
    }
    projections.insert(projections.end(), processedProj.begin(), processedProj.end());

    // Process every cell intersections once we're done with limit points.
    double t, x[3], pcoords[3], _tmp[3];
    int subId;
    for (vtkIdType i = 0; i < intersected->GetNumberOfIds(); ++i)
    {
      vtkIdType cellId = intersected->GetId(i);
      vtkCell* cell = input->GetCell(cellId);
      if (cell->GetCellDimension() == 3)
      {
        auto inOut = ::GetInOutCell3D(p1, p2, cell, tolerance);
        if (inOut.first.IsValid)
        {
          double inProj = nline.Dot((inOut.first.Position - p1) + tolerance * nline);
          double outProj = nline.Dot((inOut.second.Position - p1) - tolerance * nline);
          if (this->SamplingPattern == SamplingPattern::SAMPLE_LINE_AT_CELL_BOUNDARIES)
          {
            projections.emplace_back(ProjInfo{inProj, cellId});
            projections.emplace_back(ProjInfo{outProj, cellId});
          }
          else
          {
            projections.emplace_back(ProjInfo{(inProj + outProj) * 0.5, cellId});
          }
        }
      }
      else
      {
        cell->IntersectWithLine(p1.GetData(), p2.GetData(), tolerance, t, x, pcoords, subId);
        projections.emplace_back(ProjInfo{nline.Dot(vtkVector3d(x) - p1), cellId});
      }
    }
  }

  // Sort our array of projections so the merge across ranks is faster afterwards.
  vtkSMPTools::Sort(projections.begin(), projections.end(), &ProjInfo::SortFunction);

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
            std::vector<ProjInfo> data;
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
    std::inplace_merge(projections.begin(), prevEnd, projections.end(), &ProjInfo::SortFunction);
  }

  // Duplicate points can happen on composite data set so lets remove them
  auto last = std::unique(projections.begin(), projections.end(), &ProjInfo::EqualFunction);
  projections.erase(last, projections.end());

  ProbingPointGeneratorWorker probingPointGeneratorWorker(projections, p1, p2);
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
