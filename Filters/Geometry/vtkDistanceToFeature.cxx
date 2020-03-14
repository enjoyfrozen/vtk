/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDistanceToFeature.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDistanceToFeature.h"

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

struct DistanceData
{
  double Distance;
};

struct DistanceWorklet
{
  using LocalDataType = std::map<vtkIdType, DistanceData>;

  DistanceWorklet(
    vtkDataSet* mesh,
    vtkPolyData* features
  )
    : Mesh(mesh)
    , Features(features)
  {
    // Prepare array holding which points have been visited (none yet):
    this->Visited->SetName("visited");
    this->Visited->SetNumberOfValues(mesh->GetNumberOfPoints());
    this->Visited->FillComponent(0, 0);

    // Prepare array holding distance to feature for each points (all invalid):
    this->Distance->SetName("distance to feature");
    this->Distance->SetNumberOfValues(mesh->GetNumberOfPoints());
    this->Distance->FillComponent(0, -1.0); // Invalid, distance must be non-negative.

    if (
      !features ||
      (features->GetNumberOfCells() == features->GetNumberOfPolys() + features->GetNumberOfStrips()))
    {
      this->QueueExtremalPoints();
    }
    else
    {
      this->QueuePointsAndLines();
    }
  }

  void QueueExtremalPoints()
  {
    // I. Initialization
    //    a. Find the bounding box axis with the smallest extent.
    //    b. For each point taking on the extremal values along this axis, add to queue and mark as visited.
    // NB: Bounds reported by Mesh->GetBounds() are expected to be tight (exact) or no points will be queued.
    std::array<double, 6> bounds;
    this->Mesh->GetBounds(&bounds[0]);
    int axis = -1;
    double delta = 0.0;
    for (int aa = 0; aa < 3; ++aa)
    {
      double dd = bounds[2 * aa + 1] - bounds[2 * aa];
      if (delta == 0.0 || dd < delta)
      {
        delta = dd;
        axis = aa;
      }
    }
    if (axis < 0)
    {
      vtkGenericWarningMacro("Empty or invalid bounds.");
      // Early exit here leaves Queue empty and thus will do no work and produce no output.
      return;
    }
    std::array<double, 3> coords;
    for (vtkIdType ii = 0; ii < this->Mesh->GetNumberOfPoints(); ++ii)
    {
      this->Mesh->GetPoint(ii, &coords[0]);
      if (coords[axis] == bounds[2*axis] || coords[axis] == bounds[2 * axis + 1])
      {
        this->Visited->SetValue(ii, 1);
        this->Distance->SetValue(ii, 0.0);
        this->Queue.push_back(ii);
      }
    }
  }

  void QueuePointsAndLines()
  {
    // I. Initialization (Mesh, FeatureEdges)
    //    a. Create a map from global IDs to Mesh point IDs (gids)
    //    b. Create a vector Worklet.Queue of IDs. (this->Queue)
    std::map<vtkIdType, vtkIdType> gids;
    auto mgids = vtkIdTypeArray::SafeDownCast(this->Mesh->GetPointData()->GetGlobalIds());
    auto fgids = vtkIdTypeArray::SafeDownCast(this->Features->GetPointData()->GetGlobalIds());
    if (!mgids)
    {
      vtkGenericWarningMacro(
        "Global IDs missing for mesh (" << mgids << ") or features (" << fgids << ").");
      // Early exit here leaves Queue empty and thus will do no work and produce no output.
      return;
    }
    for (vtkIdType ii = 0; ii < mgids->GetNumberOfValues(); ++ii)
    {
      gids[mgids->GetValue(ii)] = ii;
    }
    //    c. For each point ID in FeatureEdges:
    //       i. Look up global ID, map to Mesh point ID: MPID
    //       ii. Add MPID to Worklet.Queue, mark MPID as visited with 0 distance and itself as src
    for (vtkIdType ii = 0; ii < fgids->GetNumberOfValues(); ++ii)
    {
      vtkIdType pp = gids[fgids->GetValue(ii)];
      this->Visited->SetValue(pp, 1);
      this->Distance->SetValue(pp, 0.0);
      this->Queue.push_back(pp);
    }
  }

  void Initialize()
  {
    // II. Iterate worklet, visiting neighbors until Done
    //     a. Worklet.Initialize() does nothing
  }

  void operator () (vtkIdType first, vtkIdType last)
  {
    // II. Iterate worklet, visiting neighbors until Done
    //     b. Worklet.operator() does the following:
    //        i. For each pointID in [first, last[, UpdateLocalMap(pointID), which does
    //           1. Look up cellIDs of pointID (Mesh->GetPointCells(pointID, cellsAttachedToPt))
    //           2. For each cellID, look up points defining Cell (Mesh->GetCellPoints(cellID, cellCornerPts))
    //           3. For each point2ID in cellCornerPts not already visited,
    //              set LocalMap[point2Id] = min(LocalMap entry's distance, pointID distance + d(pointID, point2ID))
    for (vtkIdType qq = first; qq < last; ++qq)
    {
      this->UpdateLocalMap(this->Queue[qq]);
    }
  }

  void UpdateLocalMap(vtkIdType pointId)
  {
    vtkNew<vtkStarIterator> star;
    vtkVector3d p0;

    this->Mesh->GetPoint(pointId, p0.GetData());
    double p0dist = this->Distance->GetValue(pointId);
    star->VisitStar1PointIds(this->Mesh, pointId, [this, &p0, &p0dist](vtkDataSet*, vtkIdType p1Id)
      {
        // Compute distance from pointId to *pointIt, possibly insert into LocalData
        vtkVector3d p1;
        this->Mesh->GetPoint(p1Id, p1.GetData());
        double p1dist = p0dist + (p1 - p0).Norm();
        if (this->Visited->GetValue(p1Id) && p1dist > this->Distance->GetValue(p1Id))
        {
          return vtkStarIterator::CONTINUE;
        }
        auto& local = this->LocalData.Local();
        auto ldit = local.find(p1Id);
        if (ldit == local.end())
        {
          local[p1Id] = DistanceData{p1dist};
        }
        else if (ldit->second.Distance > p1dist)
        {
          ldit->second.Distance = p1dist;
        }
        return vtkStarIterator::CONTINUE;
      }
    );
  }

  void Reduce()
  {
    // II. Iterate worklet, visiting neighbors until Done
    //     c. Worklet.Reduce() does the following:
    //        i. Create or erase a ReplacementQueue of point IDs.
    //        ii. Loop over TLS LocalMaps, construct union by
    //            1. accepting smallest weight as srcID for duplicateIDs (same ID in multiple LocalMaps),
    //            2. marking IDs as visited and storing smallest weight/srcID in output arrays
    //        iii. Swap Queue and ReplacementQueue

    // Mark visited points, remove old front
    /*
    for (auto ptid : this->Queue)
    {
      this->Visited->SetValue(ptid, 1);
    }
    */
    this->Queue.clear();

    // Add new front and mark distances.
    std::set<vtkIdType> front;
    vtkSMPThreadLocal<LocalDataType>::iterator ldItr;
    vtkSMPThreadLocal<LocalDataType>::iterator ldEnd = this->LocalData.end();
    for (ldItr = this->LocalData.begin(); ldItr != ldEnd; ++ldItr)
    {
      for (auto distMap = ldItr->begin(); distMap != ldItr->end(); ++distMap)
      {
        if (front.insert(distMap->first).second)
        { // Did insert (i.e., no previous entry)
          this->Visited->SetValue(distMap->first, 1);
          this->Distance->SetValue(distMap->first, distMap->second.Distance);
        }
        else
        {
          auto oldDist = this->Distance->GetValue(distMap->first);
          if (oldDist < 0 || oldDist > distMap->second.Distance)
          {
            this->Distance->SetValue(distMap->first, distMap->second.Distance);
          }
        }
      }
      // Erase entries in the local map
      ldItr->clear();
    }
    // Add all visited points to the new front:
    this->Queue.insert(this->Queue.begin(), front.begin(), front.end());
  }

  vtkDataSet* Mesh;
  vtkPolyData* Features;
  std::vector<vtkIdType> Queue; // queue of points on expanding front
  std::vector<vtkIdType> ReplacementQueue;
  vtkNew<vtkUnsignedCharArray> Visited;
  vtkNew<vtkDoubleArray> Distance;
  vtkSMPThreadLocal<LocalDataType> LocalData;
};

vtkStandardNewMacro(vtkDistanceToFeature);

vtkDistanceToFeature::vtkDistanceToFeature()
{
  this->SetNumberOfInputPorts(2);
}

vtkDistanceToFeature::~vtkDistanceToFeature()
{
}

void vtkDistanceToFeature::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkDistanceToFeature::SetSourceData(vtkPolyData* source)
{
  this->SetInputData(1, source);
}

vtkPolyData* vtkDistanceToFeature::GetSource()
{
  if (this->GetNumberOfInputConnections(1) < 1)
  {
    return nullptr;
  }
  return vtkPolyData::SafeDownCast(this->GetExecutive()->GetInputData(1, 0));
}

void vtkDistanceToFeature::SetSourceConnection(vtkAlgorithmOutput* algOutput)
{
  this->SetInputConnection(1, algOutput);
}

int vtkDistanceToFeature::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
  {
    // Source mesh to be classified may be anything that provides GetPointCells()
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  }
  else
  {
    // Feature edges must be polydata
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  }
  return 1;
}

int vtkDistanceToFeature::RequestData(
  vtkInformation*,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkInformation* inMeshInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* inFeatInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Get the inputs and output:
  auto mesh = vtkDataSet::SafeDownCast(inMeshInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto feat = vtkPolyData::SafeDownCast(inFeatInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // The output is just an annotated copy of the input.
  output->ShallowCopy(mesh);

  // I. Create a worklet to visit mesh points starting at seeded values:
  DistanceWorklet distanceToFeatures(mesh, feat);
  // II. Iterate worklet, visiting neighbors until Done
  while (!distanceToFeatures.Queue.empty())
  {
    vtkSMPTools::For(0, distanceToFeatures.Queue.size(), distanceToFeatures);
  }
  output->GetPointData()->SetScalars(distanceToFeatures.Distance);
  return 1;

}
