/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGrowCharts.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGrowCharts.h"

#include "vtkCellArray.h"
#include "vtkCellArrayIterator.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkExecutive.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSMPTools.h"
#include "vtkStarIterator.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnsignedCharArray.h"
#include "vtkVector.h"
#include "vtkVectorOperators.h"

#include <array>
#include <map>
#include <set>
#include <vector>

namespace {

/// Data held at each point on the advancing front by the ChartWorklet's threads.
struct ChartFront
{
  double Distance; // Distance to the nearest chart seed point from the front.
  vtkIdType ChartId; // The chart ID to use for the front inside Reduce().
};

/// Data for each chart used to compute persistence
struct PersistenceData
{
  double Max;
  double Min;
};

/// Thread-local data used that holds an advancing front of points and an iterator.
struct ChartData
{
  vtkNew<vtkStarIterator> Star;
  std::map<vtkIdType, ChartFront> Front;

  // These constructors and copy operators only exist so that
  // vtkSMPThreadLocal can use its Exemplar to construct a
  // new ChartData instance for each thread. (They are not
  // intended to copy existing ChartData.)
  ChartData() { }
  ChartData(const ChartData&) { }
  ChartData& operator = (const ChartData&) { return *this; }
};

/// A vtkSMPTools worklet for growing the LSCM charts via an advancing front.
struct ChartWorklet
{
  using LocalDataType = ChartData;

  ChartWorklet(
    vtkDataSet* mesh,
    vtkDataArray* seedMaxima
  )
    : Mesh(mesh)
    , SeedMax(seedMaxima)
  {
    // Prepare array holding distance to feature for each points (all invalid):
    this->Distance->SetName("distance to chart center");
    this->Distance->SetNumberOfValues(mesh->GetNumberOfPoints());
    this->Distance->FillComponent(0, -1.0); // Invalid; distance must be non-negative.

    // Prepare array holding distance to feature for each points (all invalid):
    // Note that ChartIds >= 0 implies a point has already been visited
    // (otherwise there would be no assignment).
    this->ChartIds->SetName("chart id");
    this->ChartIds->SetNumberOfValues(mesh->GetNumberOfPoints());
    this->ChartIds->FillComponent(0, -1.0); // Invalid; chart id must be non-negative.

    this->QueueExtremalDistances();
  }

  void QueueExtremalDistances()
  {
    vtkNew<vtkStarIterator> star;
    vtkIdType npts = this->Mesh->GetNumberOfPoints();
    std::set<vtkIdType> candidates;
    for (vtkIdType ii = 0; ii < npts; ++ii)
    {
      double di = this->SeedMax->GetTuple1(ii);
      bool isPotentialMax = true;
      star->VisitStar1PointIds(this->Mesh, ii, [this, di, &isPotentialMax](vtkDataSet*, vtkIdType ptId)
        {
          if (this->SeedMax->GetTuple1(ptId) > di)
          {
            isPotentialMax = false;
            return vtkStarIterator::HALT;
          }
          return vtkStarIterator::CONTINUE;
        }
      );
      if (isPotentialMax)
      {
        candidates.insert(ii);
      }
    }

    std::set<vtkIdType> notMaxima;
    vtkIdType nextChartId = 0;
    // Now eliminate "duplicate" extremal points that neighbor one another
    // std::cout << "Process " << candidates.size() << " candidates\n";
    for (auto candidate : candidates)
    {
      // std::cout << "Candidate " << candidate << "\n";
      std::set<vtkIdType> front;
      std::set<vtkIdType> all;
      // Some candidates may be part of a non-local maximum already assigned a chart ID
      // and some candidates may be non-local saddles (that look like maxima at some points
      // but eventually prove not to be at others).
      if (this->ChartIds->GetValue(candidate) >= 0 || notMaxima.find(candidate) != notMaxima.end())
      {
        continue;
      }
      // Find all connected canidates and disqualify potential candidates that are not maxima
      bool isNotMaximum = false;
      double max = this->SeedMax->GetTuple1(candidate);
      for (front.insert(candidate); !front.empty(); front.erase(front.begin()))
      {
        // std::cout << "  pop " << *front.begin() << "\n";
        if (!all.insert(*front.begin()).second)
        {
          continue;
        }
        star->VisitStar1PointIds(this->Mesh, *front.begin(),
          [this, &all, &max, &front, &candidates, &isNotMaximum](vtkDataSet*, vtkIdType ptId)
          {
            bool isCandidate = (candidates.find(ptId) != candidates.end());
            if (isCandidate)
            {
              // Make sure we don't visit any already-processed candidates
              if (all.find(ptId) == all.end())
              {
                // Insert unvisited candidate neighbors onto the front for processing
                front.insert(ptId);
                // std::cout << "    push " << ptId << "\n";
              }
            }
            else if (this->SeedMax->GetTuple1(ptId) >= max)
            {
              // If it wasn't a candidate but has a distance-to-feature
              // that matches the candidate's, then it was a potential
              // candidate that was not a local maximum, indicating we
              // should discard all the attached candidates connected to it.
              isNotMaximum = true;
            }
            return vtkStarIterator::CONTINUE;
          }
        );
      }
      if (isNotMaximum)
      {
        notMaxima.insert(all.begin(), all.end());
        // std::cout << "  " << all.size() << " entries starting with " << *all.begin() << " are not maxima\n";
      }
      else
      {
        vtkIdType chartId = nextChartId++;
        // std::cout << "  " << all.size() << " entries starting with " << *all.begin() << " are chart ID " << chartId << "\n";
        for (auto entry : all)
        {
          this->Queue.push_back(entry);
          this->Distance->SetValue(entry, 0.0);
          this->ChartIds->SetValue(entry, chartId);
        }
        vtkVector3d xx;
        this->Mesh->GetPoint(candidate, xx.GetData());
        this->SeedPoints->InsertNextPoint(xx.GetData());
        this->Persistence[chartId].Max = this->SeedMax->GetTuple1(candidate);
      }
    }
  }

  void Initialize()
  {
  }

  void operator () (vtkIdType first, vtkIdType last)
  {
    for (vtkIdType qq = first; qq < last; ++qq)
    {
      this->UpdateLocalMap(this->Queue[qq]);
    }
  }

  void UpdateLocalMap(vtkIdType pointId)
  {
    ChartData& local(this->LocalData.Local());
    vtkIdType chartId = this->ChartIds->GetValue(pointId);
    double d0 = this->Distance->GetValue(pointId);
    vtkVector3d p0;
    this->Mesh->GetPoint(pointId, p0.GetData());
    local.Star->VisitStar1PointIds(this->Mesh, pointId, [this, &p0, &d0, &local, chartId](vtkDataSet*, vtkIdType p1Id)
      {
        vtkIdType neighborChartId = this->ChartIds->GetValue(p1Id);
        vtkVector3d p1;
        this->Mesh->GetPoint(p1Id, p1.GetData());
        double d1 = d0 + (p1 - p0).Norm();
        
        if (neighborChartId >= 0 && d1 > this->Distance->GetValue(p1Id))
        {
          return vtkStarIterator::CONTINUE;
        }
        auto ldit = local.Front.find(p1Id);
        if (ldit == local.Front.end())
        {
          local.Front[p1Id] = ChartFront{d1, chartId};
        }
        else if (ldit->second.Distance > d1)
        {
          ldit->second.Distance = d1;
          ldit->second.ChartId = chartId;
        }
        return vtkStarIterator::CONTINUE;
      }
    );
  }

  void UpdatePersistence(vtkIdType chartId, vtkIdType ptId)
  {
    this->Persistence[chartId].Min = this->SeedMax->GetTuple1(ptId);
  }

  void Reduce()
  {
    this->Queue.clear();

    // Add new front and mark distances.
    std::set<vtkIdType> front;
    vtkSMPThreadLocal<LocalDataType>::iterator ldItr;
    vtkSMPThreadLocal<LocalDataType>::iterator ldEnd = this->LocalData.end();
    for (ldItr = this->LocalData.begin(); ldItr != ldEnd; ++ldItr)
    {
      for (auto chartMap = ldItr->Front.begin(); chartMap != ldItr->Front.end(); ++chartMap)
      {
        if (front.insert(chartMap->first).second)
        { // Did insert (i.e., no previous entry)
          this->Distance->SetValue(chartMap->first, chartMap->second.Distance);
          this->ChartIds->SetValue(chartMap->first, chartMap->second.ChartId);
          this->UpdatePersistence(chartMap->second.ChartId, chartMap->first);
        }
        else
        {
          auto oldDist = this->Distance->GetValue(chartMap->first);
          if (oldDist < 0 || oldDist > chartMap->second.Distance)
          {
            this->Distance->SetValue(chartMap->first, chartMap->second.Distance);
            this->ChartIds->SetValue(chartMap->first, chartMap->second.ChartId);
            this->UpdatePersistence(chartMap->second.ChartId, chartMap->first);
          }
        }
      }
      // Erase entries in the local map
      ldItr->Front.clear();
    }
    // Add all visited points to the new front:
    this->Queue.insert(this->Queue.begin(), front.begin(), front.end());
  }

  void PrepareSeedOutput(vtkPolyData* output2)
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

  vtkDataSet* Mesh;
  vtkDataArray* SeedMax;
  std::vector<vtkIdType> Queue; // queue of points on expanding front
  std::vector<vtkIdType> ReplacementQueue;
  vtkNew<vtkDoubleArray> Distance;
  vtkNew<vtkIdTypeArray> ChartIds;
  vtkNew<vtkPoints> SeedPoints;
  std::map<vtkIdType, PersistenceData> Persistence;
  vtkSMPThreadLocal<LocalDataType> LocalData;
};

} // anonymous namespace

vtkStandardNewMacro(vtkGrowCharts);

vtkGrowCharts::vtkGrowCharts()
{
  this->SetNumberOfOutputPorts(2);
}

vtkGrowCharts::~vtkGrowCharts()
{
}

void vtkGrowCharts::PrintSelf(std::ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

int vtkGrowCharts::FillInputPortInformation(int port, vtkInformation* info)
{
  // Accept starting points externally?
  return this->Superclass::FillInputPortInformation(port, info);
}

int vtkGrowCharts::FillOutputPortInformation(int port, vtkInformation* info)
{
  if (port == 1)
  {
    info->Set(vtkDataSet::DATA_TYPE_NAME(), "vtkPolyData");
    return 1;
  }
  return this->Superclass::FillOutputPortInformation(port, info);
}

int vtkGrowCharts::RequestData(
  vtkInformation*,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkInformation* inMeshInfo = inputVector[0]->GetInformationObject(0);
  // vtkInformation* inFeatInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* out2Info = outputVector->GetInformationObject(1);

  // Get the inputs and output:
  auto mesh = vtkDataSet::SafeDownCast(inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
  // auto feat = vtkPolyData::SafeDownCast(inFeatInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto output2 = vtkPolyData::SafeDownCast(out2Info->Get(vtkDataObject::DATA_OBJECT()));

  // The output is just an annotated copy of the input.
  output->ShallowCopy(mesh);

  // I. Create a worklet to visit mesh points starting at seeded values:
  // TODO: Do not hardwire the array name:
  auto dist = mesh->GetPointData()->GetArray("distance to feature");
  if (!dist)
  {
    vtkErrorMacro("Cannot run without a distance-to-feature field.");
    return 0;
  }
  ChartWorklet growCharts(mesh, dist);

  // II. Iterate worklet, visiting neighbors until Done
  while (!growCharts.Queue.empty())
  {
    vtkSMPTools::For(0, growCharts.Queue.size(), growCharts);
  }
  output->GetPointData()->SetScalars(growCharts.ChartIds);
  growCharts.PrepareSeedOutput(output2);
  return 1;
}
