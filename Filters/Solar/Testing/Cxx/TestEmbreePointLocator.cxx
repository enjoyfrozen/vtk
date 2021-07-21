/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestEmbreePointLocator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkEmbreePointLocator.h"
#include "vtkIdList.h"
#include "vtkKdTree.h"
#include "vtkKdTreePointLocator.h"
#include "vtkMath.h"
#include "vtkPointLocator.h"
#include "vtkPoints.h"
#include "vtkStructuredGrid.h"

#include "vtkTimerLog.h"

// returns true if 2 points are equidistant from x, within a tolerance
bool ArePointsEquidistant(double x[3], vtkIdType id1, vtkIdType id2, vtkPointSet* grid)
{
  if (id1 == id2)
  {
    return true;
  }
  float firstDist2 = vtkMath::Distance2BetweenPoints(x, grid->GetPoint(id1));
  float secondDist2 = vtkMath::Distance2BetweenPoints(x, grid->GetPoint(id2));

  float differenceDist2 = firstDist2 - secondDist2;
  if (differenceDist2 < 0)
  {
    differenceDist2 = -differenceDist2;
  }

  if (differenceDist2 / (firstDist2 + secondDist2) > .00001)
  {
    cerr << "Results do not match (first " << id1 << ":" << firstDist2 << " , second " << id2 << ":"
         << secondDist2 << ") " << endl;
    return false;
  }
  return true;
}

// checks that every point in firstList has a matching point (based
// on distance) in secondList
bool DoesListHaveProperPoints(
  double x[3], vtkIdList* firstList, vtkIdList* secondList, vtkPointSet* grid)
{
  for (vtkIdType uid = 0; uid < firstList->GetNumberOfIds(); uid++)
  {
    int found = 0;
    for (vtkIdType kid = 0; kid < secondList->GetNumberOfIds(); kid++)
    {
      if (firstList->GetId(uid) == secondList->GetId(kid))
      {
        found = 1;
        break;
      }
    }
    if (!found)
    {
      auto uact = firstList->GetId(uid);
      double* upt = grid->GetPoint(uact);
      cerr << "FROM 1 " << uid << " " << uact << " " << upt[0] << "," << upt[1] << "," << upt[2]
           << endl;
      for (vtkIdType kid = 0; kid < secondList->GetNumberOfIds(); kid++)
      {
        auto kact = secondList->GetId(kid);
        double* kpt = grid->GetPoint(kact);
        cerr << "FROM 2 " << kid << " " << kact << " " << kpt[0] << "," << kpt[1] << "," << kpt[2]
             << endl;
        if (ArePointsEquidistant(x, firstList->GetId(uid), secondList->GetId(kid), grid))
        {
          found = 1;
          break;
        }
      }
    }
    if (!found)
    {
      return false;
    }
  }
  return true;
}

// This test compares results for different point locators since they should
// all return the same results (within a tolerance)
int ComparePointLocators(vtkAbstractPointLocator* locator1, vtkAbstractPointLocator* locator2)
{
  int rval = 0;
  int i, j, k, kOffset, jOffset, offset;
  float x[3];
  static const int dims[3] = { 39, 31, 31 };

  // Create the structured grid.
  vtkStructuredGrid* sgrid = vtkStructuredGrid::New();
  sgrid->SetDimensions(dims);

  // We also create the points.
  vtkPoints* points = vtkPoints::New();
  points->Allocate(dims[0] * dims[1] * dims[2]);

  for (k = 0; k < dims[2]; k++)
  {
    x[2] = 1.0 + k * 1.2;
    kOffset = k * dims[0] * dims[1];
    for (j = 0; j < dims[1]; j++)
    {
      x[1] = sqrt(10. + j * 2.);
      jOffset = j * dims[0];
      for (i = 0; i < dims[0]; i++)
      {
        x[0] = 1 + i * i * .5;
        offset = i + jOffset + kOffset;
        points->InsertPoint(offset, x);
      }
    }
  }
  sgrid->SetPoints(points);
  points->Delete();

  vtkTimerLog* tl = vtkTimerLog::New();
  tl->StartTimer();
  locator1->SetDataSet(sgrid);
  locator1->BuildLocator();
  tl->StopTimer();
  cerr << "EMB BUILD " << tl->GetElapsedTime() << " ****************************" << endl;
  tl->StartTimer();
  locator2->SetDataSet(sgrid);
  locator2->BuildLocator();
  tl->StopTimer();
  cerr << "VTK BUILD " << tl->GetElapsedTime() << " ****************************" << endl;

  double bounds[6];
  sgrid->GetBounds(bounds);
  for (i = 0; i < 3; i++)
  {
    // expand the search so we are looking for points inside and outside the BB
    bounds[i * 2] *= .5;
    bounds[i * 2 + 1] *= 1.2;
  }
  int numSearchPoints = 20;
  vtkIdList* locator1List = vtkIdList::New();
  vtkIdList* locator2List = vtkIdList::New();
  for (i = 0; i < numSearchPoints; i++)
  {
    cerr << " ITERATION " << i << endl;
    double point[3] = { (bounds[0] + (bounds[1] - bounds[0]) * i / numSearchPoints),
      (bounds[2] + (bounds[3] - bounds[2]) * i / numSearchPoints),
      (bounds[4] + (bounds[5] - bounds[4]) * i / numSearchPoints) };
    cerr << "LOOKING FOR " << point[0] << "," << point[1] << "," << point[2] << endl;
    tl->StartTimer();
    vtkIdType locator1Pt = locator1->FindClosestPoint(point);
    tl->StopTimer();
    cerr << "EMB SEARCH " << tl->GetElapsedTime() << " ****************************" << endl;
    tl->StartTimer();
    vtkIdType locator2Pt = locator2->FindClosestPoint(point);
    tl->StopTimer();
    cerr << "VTK SEARCH " << tl->GetElapsedTime() << " ****************************" << endl;
    cerr << "survey says " << locator1Pt << " " << locator2Pt << endl;
    if (!ArePointsEquidistant(point, locator1Pt, locator2Pt, sgrid))
    {
      cerr << " from FindClosestPoint.\n";
      rval++;
    }
    int N = 1 + i * 250 / numSearchPoints; // test different amounts of points to search for
    cerr << "LOOKING FOR " << N << " near " << point[0] << "," << point[1] << "," << point[2]
         << endl;
    locator1->FindClosestNPoints(N, point, locator1List);
    locator2->FindClosestNPoints(N, point, locator2List);
    if (!ArePointsEquidistant(point, locator1Pt, locator1List->GetId(0), sgrid))
    {
      cerr
        << "for comparing FindClosestPoint and first result of FindClosestNPoints for locator1.\n";
      rval++;
    }
    if (!ArePointsEquidistant(point, locator2Pt, locator2List->GetId(0), sgrid))
    {
      cerr
        << "for comparing FindClosestPoint and first result of FindClosestNPoints for locator2.\n";
      rval++;
    }

    for (j = 0; j < N; j++)
    {
      if (!ArePointsEquidistant(point, locator1List->GetId(j), locator2List->GetId(j), sgrid))
      {
        cerr << "for point " << j << " for ClosestNPoints search.\n";
        rval++;
      }
    }
    double radius = 10;
    cerr << "Looking within " << radius << " of " << point[0] << "," << point[1] << "," << point[2]
         << endl;
    locator1->FindPointsWithinRadius(radius, point, locator1List);
    locator2->FindPointsWithinRadius(radius, point, locator2List);
    cerr << "emb vs vtk " << endl;
    cerr << (DoesListHaveProperPoints(point, locator1List, locator2List, sgrid) ? "PROPER"
                                                                                : "IMPROPER")
         << endl;
    cerr << "vtk vs emb " << endl;
    cerr << (DoesListHaveProperPoints(point, locator2List, locator1List, sgrid) ? "PROPER"
                                                                                : "IMPROPER")
         << endl;
    if (!DoesListHaveProperPoints(point, locator1List, locator2List, sgrid) ||
      !DoesListHaveProperPoints(point, locator2List, locator1List, sgrid))
    {
      cerr << "Problem with FindPointsWithinRadius\n";
      rval++;
    }
    cerr << "Looking for closest to " << point[0] << "," << point[1] << "," << point[2]
         << " within " << radius << endl;
    double dist2;
    locator1Pt = locator1->FindClosestPointWithinRadius(radius, point, dist2);
    locator2Pt = locator2->FindClosestPointWithinRadius(radius, point, dist2);
    cerr << "SURVEY SAYS " << locator1Pt << " " << locator1Pt << endl;
    if (locator1Pt < 0 || locator2Pt < 0)
    {
      if (locator1Pt >= 0 || locator2Pt >= 0)
      {
        if (locator1Pt < 0)
          cerr << "L1 < 0 " << endl;
        if (locator2Pt < 0)
          cerr << "L2 < 0 " << endl;
        if (locator1Pt >= 0)
          cerr << "L1 >= 0 " << endl;
        if (locator2Pt >= 0)
          cerr << "L2 >= 0 " << endl;
        cerr << "Inconsistent results for FindClosestPointWithinRadius\n";
        rval++;
      }
    }
    else if (!ArePointsEquidistant(point, locator1Pt, locator2Pt, sgrid))
    {
      cerr << "Incorrect result for FindClosestPointWithinRadius.\n";
      rval++;
    }
    if (locator1Pt >= 0)
    {
      locator1List->Reset();
      locator1List->InsertNextId(locator1Pt);
      if (!DoesListHaveProperPoints(point, locator1List, locator2List, sgrid))
      {
        cerr << "Inconsistent results for FindClosestPointWithinRadius and FindPointsWithRadius\n";
        rval++;
      }
    }
  }

  locator1List->Delete();
  locator2List->Delete();

  sgrid->Delete();

  return rval; // returns 0 if all tests passes
}

int TestEmbreePointLocator(int, char*[])
{
  vtkKdTreePointLocator* kdTreeLocator = vtkKdTreePointLocator::New();
  vtkPointLocator* uniformLocator = vtkPointLocator::New();

  int rval = 0;
  // cout << "Comparing vtkPointLocator to vtkKdTreePointLocator.\n";
  // rval = ComparePointLocators(uniformLocator, kdTreeLocator);

  vtkEmbreePointLocator* embreeLocator = vtkEmbreePointLocator::New();
  cerr << "Comparing vtkEmbreePointLocator to vtkKdTreePointLocator.\n";
  rval += ComparePointLocators(embreeLocator, kdTreeLocator);

  kdTreeLocator->Delete();
  uniformLocator->Delete();
  embreeLocator->Delete();

  return rval;
}
