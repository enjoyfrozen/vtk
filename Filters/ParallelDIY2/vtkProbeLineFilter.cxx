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
#include "vtkCellLocatorStrategy.h"
#include "vtkCompositeDataSet.h"
#include "vtkCutter.h"
#include "vtkDIYExplicitAssigner.h"
#include "vtkDIYUtilities.h"
#include "vtkDataArrayRange.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkFindCellStrategy.h"
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
#include <map>
#include <utility>
#include <vector>

// clang-format off
#include "vtk_diy2.h"
#include VTK_DIY2(diy/master.hpp)
#include VTK_DIY2(diy/mpi.hpp)
// clang-format off

vtkStandardNewMacro(vtkProbeLineFilter);

vtkCxxSetObjectMacro(vtkProbeLineFilter, Controller, vtkMultiProcessController);

//------------------------------------------------------------------------------
struct vtkProbeLineFilter::vtkInternals
{
  vtkMTimeType PreviousInputTime = 0;
  std::map<vtkDataSet*,vtkSmartPointer<vtkFindCellStrategy>> Strategies;

  void UpdateLocators(vtkDataObject* input, int pattern, const double tolerance)
  {
    vtkMTimeType inputTime = input->GetMTime();
    bool isInputDifferent = inputTime != this->PreviousInputTime;
    bool needLocators =
      pattern == vtkProbeLineFilter::SAMPLE_LINE_AT_CELL_BOUNDARIES
      || pattern == vtkProbeLineFilter::SAMPLE_LINE_AT_SEGMENT_CENTERS;
    if (isInputDifferent && needLocators)
    {
      this->PreviousInputTime = inputTime;

      const auto& inputs = vtkCompositeDataSet::GetDataSets(input);
      for (vtkDataSet* ds : inputs)
      {
        vtkNew<vtkStaticCellLocator> locator;
        locator->SetDataSet(ds);
        locator->UseDiagonalLengthToleranceOff();
        locator->SetTolerance(tolerance);
        locator->BuildLocator();

        vtkCellLocatorStrategy* strategy = vtkCellLocatorStrategy::New();
        strategy->SetCellLocator(locator);

        this->Strategies[ds] = vtkSmartPointer<vtkFindCellStrategy>::Take(static_cast<vtkFindCellStrategy*>(strategy));
      }
    }
  }
};

namespace
{
//==============================================================================
/**
 * Store the information of the intersection between a cell and a ray. InT and OutT
 * are the parameteric distances on the ray for the first (and second for 3D cells)
 * intersection between the ray and the cell. CellId is the id of the intersected cell.
 * If -1 then express intersection happening outside a cell.
 */
struct HitCellInfo
{
  double InT;
  double OutT;
  vtkIdType CellId;

  operator bool() const noexcept
  {
    return this->InT >= 0.0 && this->OutT >= 0.0;
  }

  bool operator<(const HitCellInfo& r) const noexcept
  {
    return this->InT < r.InT;
  }
};

//==============================================================================
/**
 * Return the entry point and exit point (respectively result.first and result.second) of a given 3D cell for
 * the segment [p1,p2].
 */
HitCellInfo GetInOutCell3D(const vtkVector3d& p1, const vtkVector3d& p2, vtkCell* cell, double tolerance)
{
  double t, x[3], dummy3[3];
  int dummy;
  HitCellInfo res{-1.0, -1.0, -1};

  if (cell->IntersectWithLine(p1.GetData(), p2.GetData(), 0.0, t, x, dummy3, dummy))
  {
    res.InT = t;
  }
  if (cell->IntersectWithLine(p2.GetData(), p1.GetData(), 0.0, t, x, dummy3, dummy))
  {
    res.OutT = 1.0 - t;
  }

  return res;
}

//==============================================================================
/**
 * Return the intersection of a point p1 with a cell in an input dataset (and its locator).
 * Also return the intersection from this point to the closest surface in the direction
 * of p2.
 */
HitCellInfo ProcessLimitPoint(vtkVector3d p1, vtkVector3d p2, int pattern, vtkDataSet* input, vtkAbstractCellLocator* locator, double tolerance)
{
  const double norm = (p2 - p1).Norm();
  HitCellInfo result{0.0, -1.0, -1};

  vtkIdType cellId = locator->FindCell(p1.GetData());
  if (cellId >= 0)
  {
    vtkCell* cell = input->GetCell(cellId);
    result.CellId = cellId;
    if (cell->GetCellDimension() == 3)
    {
      double outT = ::GetInOutCell3D(p1, p2, cell, tolerance).OutT;
      result.OutT = outT - tolerance / norm;
    }
    else
    {
      result.OutT = 0.0;
    }
  }
  else if (pattern == vtkProbeLineFilter::SamplingPattern::SAMPLE_LINE_AT_CELL_BOUNDARIES)
  {
    double t, x[3], pcoords[3];
    int id;
    if (locator->IntersectWithLine(p1.GetData(), p2.GetData(), tolerance, t, x, pcoords, id))
    {
      result.OutT = t - tolerance / norm;
    }
  }

  return result;
}

//==============================================================================
/**
 * Workers to project back intersections from their parametric representation to
 * actual 3D coordinates.
 */
struct PointProjectionBordersWorker
{
  PointProjectionBordersWorker(const vtkVector3d& p1, const vtkVector3d& p2, const std::vector<HitCellInfo>& inter, vtkIdType offset, vtkPoints* result)
    : P1(p1)
    , V12(p2 - p1)
    , Intersections(inter)
    , Offset(offset)
    , Result(result)
  {}

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    vtkVector3d point;
    vtkIdType idx = this->Offset + 2 * startId;
    for (vtkIdType i = startId; i < endId; ++i)
    {
      point = this->P1 + this->Intersections[i].InT * this->V12;
      this->Result->SetPoint(idx, point.GetData());
      ++idx;
      point = this->P1 + this->Intersections[i].OutT * this->V12;
      this->Result->SetPoint(idx, point.GetData());
      ++idx;
    }
  }

  const vtkVector3d P1;
  const vtkVector3d V12;
  const std::vector<HitCellInfo>& Intersections;
  vtkIdType Offset;
  vtkPoints* Result;
};

struct PointProjectionCentersWorker
{
  PointProjectionCentersWorker(const vtkVector3d& p1, const vtkVector3d& p2, const std::vector<HitCellInfo>& inter, vtkPoints* result)
    : P1(p1)
    , V12(p2 - p1)
    , Intersections(inter)
    , Result(result)
  {}

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    vtkVector3d point;
    for (vtkIdType i = startId; i < endId; ++i)
    {
      point = this->P1 + (this->Intersections[i].InT + this->Intersections[i].OutT) * 0.5 * this->V12;
      this->Result->SetPoint(i + 1, point.GetData());
    }
  }

  const vtkVector3d P1;
  const vtkVector3d V12;
  const std::vector<HitCellInfo>& Intersections;
  vtkPoints* Result;
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
  , Internal(new vtkInternals)
{
  this->SetNumberOfInputPorts(1);
  this->SetController(vtkMultiProcessController::GetGlobalController());
}

//------------------------------------------------------------------------------
vtkProbeLineFilter::~vtkProbeLineFilter()
{
  this->SetController(nullptr);
  delete this->Internal;
}

//------------------------------------------------------------------------------
int vtkProbeLineFilter::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  if (!outInfo || !inInfo)
  {
    vtkErrorMacro("No input or output information");
  }

  const double tolerance = this->ComputeTolerance
    ? VTK_TOL * (vtkVector3d(this->Point2) - vtkVector3d(this->Point1)).Norm()
    : this->Tolerance;

  vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkDataSet* output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  this->Internal->UpdateLocators(input, this->SamplingPattern, tolerance);

  vtkSmartPointer<vtkPolyData> sampledLine;
  switch (this->SamplingPattern)
  {
    case SAMPLE_LINE_AT_CELL_BOUNDARIES:
    case SAMPLE_LINE_AT_SEGMENT_CENTERS:
      sampledLine = this->SampleLineAtEachCell(vtkCompositeDataSet::GetDataSets(input), tolerance);
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
  prober->SetFindCellStrategyMap(this->Internal->Strategies);
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
  const std::vector<vtkDataSet*>& inputs, const double tolerance) const
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

  vtkVector3d p1{this->Point1};
  vtkVector3d p2{this->Point2};
  vtkVector3d v12Epsilon = p2 - p1;
  const double v12NormEpsilon = tolerance / v12Epsilon.Normalize();
  v12Epsilon = v12Epsilon * tolerance;
  std::vector<HitCellInfo> intersections;

  // Add every intersection with all blocks of the dataset on our current rank.
  // First loop on all block of the input
  for (std::size_t dsId = 0; dsId < inputs.size(); ++dsId)
  {
    vtkDataSet* input = inputs[dsId];
    auto* strategy = vtkCellLocatorStrategy::SafeDownCast(this->Internal->Strategies.at(input));
    assert(strategy);
    vtkAbstractCellLocator* locator = strategy->GetCellLocator();

    vtkNew<vtkIdList> intersectedIds;
    locator->FindCellsAlongLine(this->Point1, this->Point2, 0.0, intersectedIds);

    // We process p1 and p2 a bit differently so in the case of their intersection with a cell
    // they are not duplicated
    auto AddLimitPointToIntersections = [&](const vtkVector3d& p1, const vtkVector3d& p2, bool inverse)
    {
      auto processed = ::ProcessLimitPoint(p1, p2, this->SamplingPattern, input, locator, tolerance);

      if (processed.OutT >= 0.0)
      {
        if (inverse)
        {
          processed.InT = 1.0 - processed.OutT;
          processed.OutT = 1.0; // - processed.InT (== 0.0)
        }
        intersections.emplace_back(processed);

        if (processed.CellId >= 0.0)
        {
          intersectedIds->DeleteId(processed.CellId);
        }
      }
    };
    AddLimitPointToIntersections(p1, p2, false);
    AddLimitPointToIntersections(p2, p1, true);

    // Process every cell intersection once we're done with limit points
    double t, x[3], pcoords[3];
    int subId;
    for (vtkIdType i = 0; i < intersectedIds->GetNumberOfIds(); ++i)
    {
      vtkIdType cellId = intersectedIds->GetId(i);
      if (input->HasAnyGhostCells() && input->GetCellGhostArray()->GetValue(cellId))
      {
        continue;
      }
      vtkCell* cell = input->GetCell(cellId);
      auto inOut = ::GetInOutCell3D(p1, p2, cell, tolerance);
      inOut.CellId = cellId;

      // Handle degenerated intersection
      if (!inOut || vtkMathUtilities::NearlyEqual(inOut.InT, inOut.OutT, tolerance))
      {
        continue;
      }

      // Add intersected cell
      if (cell->GetCellDimension() == 3)
      {
        inOut.InT += v12NormEpsilon;
        inOut.OutT -= v12NormEpsilon;
      }

      intersections.emplace_back(inOut);
    }
  }

  // // Sort our array of projections so the merge across ranks is faster afterwards.
  vtkSMPTools::Sort(intersections.begin(), intersections.end());

  // We need to gather points from every ranks to every ranks because vtkProbeFilter
  // assumes that its input is replicated in every ranks.
  using PointSetBlock = std::vector<std::vector<HitCellInfo>>;
  diy::mpi::communicator comm = vtkDIYUtilities::GetCommunicator(this->Controller);
  diy::Master master(
    comm, 1, -1, []() { return static_cast<void*>(new PointSetBlock()); },
    [](void* b) -> void { delete static_cast<PointSetBlock*>(b); });
  vtkDIYExplicitAssigner assigner(comm, 1);
  diy::RegularDecomposer<diy::DiscreteBounds> decomposer(
    /*dim*/ 1, diy::interval(0, assigner.nblocks() - 1), assigner.nblocks());
  decomposer.decompose(comm.rank(), assigner, master);

  diy::all_to_all(
    master, assigner, [&master, &intersections](PointSetBlock* block, const diy::ReduceProxy& srp) {
      int myBlockId = srp.gid();
      if (srp.round() == 0)
      {
        for (int i = 0; i < srp.out_link().size(); ++i)
        {
          const diy::BlockID& blockId = srp.out_link().target(i);
          if (blockId.gid != myBlockId)
          {
            srp.enqueue(blockId, intersections);
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
            std::vector<HitCellInfo> data;
            srp.dequeue(blockId, data);
            block->emplace_back(std::move(data));
          }
        }
      }
    });

  // Merge local intersections with intersections from all other ranks
  PointSetBlock* block = master.block<PointSetBlock>(0);
  double p1Hit = 1.0;
  double p2Hit = 0.0;
  auto ReduceLimitPointHit = [&p1Hit, &p2Hit](const std::vector<HitCellInfo>& intersections)
  {
    if (intersections[0].CellId < 0 && intersections[0].OutT < p1Hit)
    {
      p1Hit = intersections[0].OutT;
    }
    if (intersections.back().CellId < 0 && intersections.back().InT > p2Hit)
    {
      p2Hit = intersections.back().InT;
    }
  };
  if (!intersections.empty())
  {
    ReduceLimitPointHit(intersections);
    if (intersections.back().CellId < 0)
    {
      intersections.pop_back();
    }
    if (!intersections.empty() && intersections[0].CellId < 0)
    {
      intersections.erase(intersections.begin());
    }
  }

  for (const auto& distIntersections : *block)
  {
    if (!distIntersections.empty())
    {
      ReduceLimitPointHit(distIntersections);
      auto begin = distIntersections.begin();
      auto end = distIntersections.end();
      if (distIntersections[0].CellId < 0)
      {
        begin += 1;
      }
      if (distIntersections.back().CellId < 0)
      {
        end -= 1;
      }

      if (begin < end)
      {
        auto prevEnd = intersections.insert(intersections.end(), begin, end);
        std::inplace_merge(intersections.begin(), prevEnd, intersections.end());
      }
    }
  }

  if (intersections.empty())
  {
    // In this instance, we probe only Point1 and Point2.
    vtkNew<vtkLineSource> line;
    line->SetPoint1(this->Point1);
    line->SetPoint2(this->Point2);
    line->Update();
    return vtkPolyData::SafeDownCast(line->GetOutputDataObject(0));
  }

  // Tranform back the cells hit informations to 3D coordinates
  vtkNew<vtkPoints> coordinates;
  vtkVector3d point;
  vtkVector3d v12 = p2 - p1;
  if (this->SamplingPattern == SAMPLE_LINE_AT_CELL_BOUNDARIES)
  {
    bool addP1 = intersections[0].InT != 0.0;
    bool addP2 = intersections.back().OutT != 1.0;
    vtkIdType offset = addP1 ? 2 : 0;
    vtkIdType numberOfPoints = intersections.size() * 2 + offset + (addP2 ? 2 : 0);
    coordinates->SetNumberOfPoints(numberOfPoints);
    if (addP1)
    {
      point = p1 + p1Hit * v12;
      coordinates->SetPoint(0, this->Point1);
      coordinates->SetPoint(1, point.GetData());
    }
    ::PointProjectionBordersWorker worker(p1, p2, intersections, offset, coordinates);
    vtkSMPTools::For(0, intersections.size(), worker);
    if (addP2)
    {
      point = p1 + p2Hit * v12;
      coordinates->SetPoint(numberOfPoints - 2, point.GetData());
      coordinates->SetPoint(numberOfPoints - 1, this->Point2);
    }
  }
  else
  {
    coordinates->SetNumberOfPoints(intersections.size() + 2);
    coordinates->SetPoint(0, this->Point1);
    ::PointProjectionCentersWorker worker(p1, p2, intersections, coordinates);
    vtkSMPTools::For(0, intersections.size(), worker);
    coordinates->SetPoint(intersections.size() + 1, this->Point2);
  }

  vtkNew<vtkPolyLineSource> polyLine;
  polyLine->SetPoints(coordinates);
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
