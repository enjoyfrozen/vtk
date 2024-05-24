// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkGrowCharts.h"

#include "vtkCellArray.h"
#include "vtkCellArrayIterator.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkExecutive.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSMPTools.h"
#include "vtkStarIterator.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkVector.h"
#include "vtkVectorOperators.h"

#include <algorithm>
#include <array>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

VTK_ABI_NAMESPACE_BEGIN

namespace
{
//==============================================================================
struct Hash
{
  std::size_t operator()(const std::pair<vtkIdType, vtkIdType>& p) const
  {
    return std::hash<vtkIdType>{}(p.first) ^ std::hash<vtkIdType>{}(p.second);
  }
};

//==============================================================================
/// Data held at each point on the advancing front by the ChartWorklet's threads.
struct ChartFront
{
  double Distance;   // Distance to the nearest chart seed point from the front.
  vtkIdType ChartId; // The chart ID to use for the front inside Reduce().
};

//==============================================================================
/// Data for each chart used to compute persistence
struct PersistenceData
{
  double Max;
  double Min;
};

//==============================================================================
/// Thread-local data used that holds an advancing front of points and an iterator.
struct ChartData
{
  vtkNew<vtkStarIterator> Star;
  std::map<vtkIdType, ChartFront> Front;

  // These constructors and copy operators only exist so that
  // vtkSMPThreadLocal can use its Exemplar to construct a
  // new ChartData instance for each thread. (They are not
  // intended to copy existing ChartData.)
  ChartData() = default;
  ChartData(const ChartData& /*unused*/) {}
  ChartData& operator=(const ChartData& /*unused*/) { return *this; }
};

//==============================================================================
/// A vtkSMPTools worklet for growing the LSCM charts via an advancing front.
struct ChartWorklet
{
  using LocalDataType = ChartData;

  ChartWorklet(vtkDataSet* mesh, vtkDataArray* seedMaxima);
  void QueueExtremalDistances();
  void UpdatePersistence(vtkIdType chartId, vtkIdType ptId);
  void PrepareSeedOutput(vtkPolyData* output2);

  void operator()();

  vtkIdType GetOppositeFaceId(vtkIdType a, vtkIdType b, vtkIdType faceId);

  enum
  {
    DIST,
    FACETS,
    HALF_EDGE
  };

  vtkDataSet* Mesh;
  vtkDataArray* SeedMax;
  using HalfEdge = std::pair<vtkIdType, vtkIdType>;
  using Facets = std::pair<vtkIdType, vtkIdType>;
  using DistHalfEdge = std::tuple<double, Facets, HalfEdge>;
  std::priority_queue<DistHalfEdge> Heap;
  std::unordered_set<HalfEdge, Hash> ChartBoundaries;
  vtkNew<vtkIdTypeArray> ChartIds;
  vtkNew<vtkPoints> SeedPoints;
  std::map<vtkIdType, PersistenceData> Persistence;
  vtkSMPThreadLocal<LocalDataType> LocalData;
  std::unordered_multimap<vtkIdType, HalfEdge> CellToEdges;
  std::unordered_multimap<vtkIdType, vtkIdType> CellToCells;
  std::unordered_multimap<vtkIdType, vtkIdType> PointToCells;
  vtkNew<vtkDoubleArray> MaxDistanceToFeature;
  double Epsilon;
};

//------------------------------------------------------------------------------
ChartWorklet::ChartWorklet(vtkDataSet* mesh, vtkDataArray* seedMaxima)
  : Mesh(mesh)
  , SeedMax(seedMaxima)
{
  // Prepare array holding distance to feature for each points (all invalid):

  // Prepare array holding distance to feature for each points (all invalid):
  // Note that ChartIds >= 0 implies a point has already been visited
  // (otherwise there would be no assignment).
  this->ChartIds->SetName(vtkGrowCharts::ChartIdArrayName());
  this->ChartIds->SetNumberOfValues(mesh->GetNumberOfCells());
  this->ChartIds->Fill(-10000); // Invalid; chart id must be non-negative.

  this->QueueExtremalDistances();
}

//------------------------------------------------------------------------------
vtkIdType ChartWorklet::GetOppositeFaceId(vtkIdType a, vtkIdType b, vtkIdType faceId)
{
  std::unordered_set<vtkIdType> visited;
  auto aRange = this->PointToCells.equal_range(a);
  auto bRange = this->PointToCells.equal_range(b);
  for (auto it = aRange.first; it != aRange.second; ++it)
  {
    visited.emplace(it->second);
  }
  for (auto it = bRange.first; it != bRange.second; ++it)
  {
    if (it->second != faceId && visited.count(it->second))
    {
      return it->second;
    }
  }
  vtkLog(ERROR, "Error looking for opposite face.");
  return -1;
}

//------------------------------------------------------------------------------
ChartWorklet::HalfEdge CreateHalfEdge(vtkIdType a, vtkIdType b)
{
  return ChartWorklet::HalfEdge(std::min(a, b), std::max(a, b));
}

//------------------------------------------------------------------------------
void ChartWorklet::QueueExtremalDistances()
{
  std::unordered_set<HalfEdge, Hash> stored;
  vtkIdType ncells = this->Mesh->GetNumberOfCells();
  vtkNew<vtkIdList> pointIds;
  vtkNew<vtkIdList> cellIds;
  vtkIdType currentChart = -1;
  double maxDist = 0;

  for (vtkIdType pointId = 0; pointId < this->Mesh->GetNumberOfPoints(); ++pointId)
  {
    this->Mesh->GetPointCells(pointId, cellIds);
    for (vtkIdType i = 0; i < cellIds->GetNumberOfIds(); ++i)
    {
      this->PointToCells.emplace(pointId, cellIds->GetId(i));
    }
  }

  for (vtkIdType cellId = 0; cellId < ncells; ++cellId)
  {
    this->Mesh->GetCellPoints(cellId, pointIds);

    auto insertFaceEdge = [&](vtkIdType a, vtkIdType b) {
      this->CellToEdges.emplace(cellId, CreateHalfEdge(a, b));
    };
    switch (this->Mesh->GetCellType(cellId))
    {
      case VTK_PIXEL:
        insertFaceEdge(pointIds->GetId(0), pointIds->GetId(1));
        insertFaceEdge(pointIds->GetId(1), pointIds->GetId(3));
        insertFaceEdge(pointIds->GetId(2), pointIds->GetId(3));
        insertFaceEdge(pointIds->GetId(2), pointIds->GetId(0));

        break;
      default:
        vtkIdType n = pointIds->GetNumberOfIds();
        for (vtkIdType i = 0; i < n - 1; ++i)
        {
          insertFaceEdge(pointIds->GetId(i), pointIds->GetId(i + 1));
        }
        insertFaceEdge(pointIds->GetId(0), pointIds->GetId(n - 1));
        break;
    }
  }

  for (vtkIdType faceId = 0; faceId < ncells; ++faceId)
  {
    std::unordered_set<vtkIdType> visited;
    double di = this->SeedMax->GetTuple1(faceId);
    bool isSeed = true;
    this->Mesh->GetCellPoints(faceId, pointIds);

    for (vtkIdType j = 0; j < pointIds->GetNumberOfIds(); ++j)
    {
      vtkIdType pointId = pointIds->GetId(j);
      this->Mesh->GetPointCells(pointId, cellIds);
      for (vtkIdType i = 0; i < cellIds->GetNumberOfIds(); ++i)
      {
        vtkIdType cellId = cellIds->GetId(i);
        if (cellId == faceId)
        {
          continue;
        }
        if (visited.count(cellId))
        {
          continue;
        }
        visited.emplace(cellId);
        if (di < this->SeedMax->GetTuple1(cellId))
        {
          isSeed = false;
          break;
        }
      }
      if (!isSeed)
      {
        break;
      }
    }
    if (isSeed)
    {
      auto insertHalfEdges = [&](vtkIdType a, vtkIdType b) {
        vtkIdType oppositeFaceId = this->GetOppositeFaceId(a, b, faceId);
        HalfEdge e = CreateHalfEdge(a, b);
        if (stored.count(e))
        {
          return;
        }
        this->Heap.emplace(di, Facets(faceId, oppositeFaceId), e);
        stored.emplace(std::move(e));
        this->ChartIds->SetValue(faceId, currentChart);
        std::unordered_set<vtkIdType> hull;
        for (vtkIdType i = 0; i < pointIds->GetNumberOfIds(); ++i)
        {
          hull.emplace(pointIds->GetId(i));
        }
        this->MaxDistanceToFeature->InsertNextValue(di);
        maxDist = std::max(maxDist, di);
      };

      ++currentChart;
      switch (this->Mesh->GetCellType(faceId))
      {
        case VTK_PIXEL:
          insertHalfEdges(pointIds->GetId(0), pointIds->GetId(1));
          insertHalfEdges(pointIds->GetId(1), pointIds->GetId(3));
          insertHalfEdges(pointIds->GetId(2), pointIds->GetId(3));
          insertHalfEdges(pointIds->GetId(0), pointIds->GetId(2));
          break;
        default:
          vtkIdType n = pointIds->GetNumberOfIds();
          for (vtkIdType edgeId = 0; edgeId < n - 1; ++edgeId)
          {
            insertHalfEdges(pointIds->GetId(edgeId), pointIds->GetId(edgeId + 1));
          }
          insertHalfEdges(pointIds->GetId(0), pointIds->GetId(n - 1));
          break;
      }
    }
  }
  this->Epsilon = maxDist / 4;
}

//------------------------------------------------------------------------------
void ChartWorklet::operator()()
{
  std::vector<int> chartMap(this->Heap.size());
  std::iota(chartMap.begin(), chartMap.end(), 0);
  std::stack<int> s;

  // Get the chart id.
  // We have to hop until we find chartId == chartMap[chartId] because of merged charts.
  // This graph will always land on the chart id that "owns" the chart
  // (it's the chart of greatest distance to feature)
  auto getChartId = [&](vtkIdType chartId) {
    if (chartId < 0)
    {
      return chartId;
    }
    while (chartMap[chartId] != chartId)
    {
      s.push(chartId);
      chartId = chartMap[chartId];
    }
    while (!s.empty())
    {
      chartMap[s.top()] = chartId;
      s.pop();
    }
    return chartId;
  };

  vtkNew<vtkIdList> pointIds;
  while (!this->Heap.empty())
  {
    DistHalfEdge halfEdge = std::move(this->Heap.top());
    this->Heap.pop();
    const Facets& facets = std::get<FACETS>(halfEdge);
    vtkIdType faceId = facets.first;
    vtkIdType oppositeFaceId = facets.second;

    if (oppositeFaceId < 0)
    {
      continue;
    }

    int chartId = getChartId(this->ChartIds->GetValue(faceId));
    int oppositeChartId = getChartId(this->ChartIds->GetValue(oppositeFaceId));

    // If chart(Fopp) is undefined
    if (this->ChartIds->GetValue(oppositeFaceId) < 0)
    {
      // Add Fopp to chart(F)
      this->ChartIds->SetValue(oppositeFaceId, chartId);

      // Insert back edges to heap so we can continue the exploration
      auto range = this->CellToEdges.equal_range(oppositeFaceId);
      for (auto it = range.first; it != range.second; ++it)
      {
        this->Heap.emplace(this->SeedMax->GetTuple1(oppositeFaceId),
          Facets(oppositeFaceId,
            this->GetOppositeFaceId(it->second.first, it->second.second, oppositeFaceId)),
          it->second);
      }
    }
    else if (chartId != oppositeChartId &&
      this->MaxDistanceToFeature->GetValue(chartId) - this->SeedMax->GetTuple1(faceId) <
        this->Epsilon &&
      this->MaxDistanceToFeature->GetValue(oppositeChartId) - this->SeedMax->GetTuple1(faceId) <
        this->Epsilon)
    {
      // We merge the 2 charts.
      // Simply writing on chartMap is sufficient to merge charts as we always query
      // it from the ChartIds array to know what is our actual chart.
      if (this->MaxDistanceToFeature->GetValue(chartId) >
        this->MaxDistanceToFeature->GetValue(oppositeChartId))
      {
        chartMap[oppositeChartId] = chartId;
      }
      else
      {
        chartMap[chartId] = oppositeChartId;
      }
    }
  }

  for (vtkIdType cellId = 0; cellId < this->Mesh->GetNumberOfCells(); ++cellId)
  {
    this->ChartIds->SetValue(cellId, chartMap[this->ChartIds->GetValue(cellId)]);
  }
}
//------------------------------------------------------------------------------
void ChartWorklet::PrepareSeedOutput(vtkPolyData* output2)
{
  output2->SetPoints(this->SeedPoints);
  vtkIdType nn = this->SeedPoints->GetNumberOfPoints();
  vtkNew<vtkDoubleArray> persist;
  persist->SetNumberOfTuples(nn);
  persist->SetName("persistence");
  for (vtkIdType ii = 0; ii < nn; ++ii)
  {
    persist->SetValue(ii, this->Persistence[ii].Max - this->Persistence[ii].Min);
  }
  output2->GetPointData()->SetScalars(persist);
}
} // anonymous namespace

vtkStandardNewMacro(vtkGrowCharts);

//------------------------------------------------------------------------------
vtkGrowCharts::vtkGrowCharts()
{
  this->SetNumberOfOutputPorts(2);
}

//------------------------------------------------------------------------------
vtkGrowCharts::~vtkGrowCharts() = default;

void vtkGrowCharts::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
int vtkGrowCharts::FillInputPortInformation(int port, vtkInformation* info)
{
  // Accept starting points externally?
  return this->Superclass::FillInputPortInformation(port, info);
}

//------------------------------------------------------------------------------
int vtkGrowCharts::RequestData(vtkInformation* /*request*/, vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkInformation* inMeshInfo = inputVector[0]->GetInformationObject(0);
  // vtkInformation* inFeatInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* out2Info = outputVector->GetInformationObject(1);

  // Get the inputs and output:
  auto* mesh = vtkDataSet::SafeDownCast(inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
  // auto feat = vtkPolyData::SafeDownCast(inFeatInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto* output2 = vtkPolyData::SafeDownCast(out2Info->Get(vtkDataObject::DATA_OBJECT()));

  // The output is just an annotated copy of the input.
  output->ShallowCopy(mesh);

  auto* dist = this->GetInputArrayToProcess(0, inputVector);
  if (!dist)
  {
    vtkErrorMacro("Cannot run without a distance-to-feature field.");
    return 0;
  }
  ChartWorklet growCharts(mesh, dist);
  growCharts();

  output->GetCellData()->SetScalars(growCharts.ChartIds);
  output->GetCellData()->AddArray(dist);
  growCharts.PrepareSeedOutput(output2);
  return 1;
}
VTK_ABI_NAMESPACE_END
