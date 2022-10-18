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
      for (vtkIdType kid = 0; kid < secondList->GetNumberOfIds(); kid++)
      {
        auto kact = secondList->GetId(kid);
        double* kpt = grid->GetPoint(kact);
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
int ComparePointLocators(vtkAbstractPointLocator* locator1, std::string name1,
  vtkAbstractPointLocator* locator2, std::string name2)
{
  int i, j, k, kOffset, jOffset, offset;
  float x[3];
  static const int dims[3] = { 39, 31, 31 };

  // Create the structured grid.
  vtkNew<vtkStructuredGrid> sgrid;
  sgrid->SetDimensions(dims);

  // We also create the points.
  vtkNew<vtkPoints> points;
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

  vtkNew<vtkTimerLog> tl;

  locator1->SetDataSet(sgrid);
  locator1->BuildLocator();

  locator2->SetDataSet(sgrid);
  locator2->BuildLocator();

  double bounds[6];
  sgrid->GetBounds(bounds);
  for (i = 0; i < 3; i++)
  {
    // expand the search so we are looking for points inside and outside the BB
    bounds[i * 2] *= .5;
    bounds[i * 2 + 1] *= 1.2;
  }

  int numSearchPoints = 20;
  vtkNew<vtkIdList> locator1List;
  vtkNew<vtkIdList> locator2List;

  for (i = 0; i < numSearchPoints; i++)
  {
    double point[3] = { (bounds[0] + (bounds[1] - bounds[0]) * i / numSearchPoints),
      (bounds[2] + (bounds[3] - bounds[2]) * i / numSearchPoints),
      (bounds[4] + (bounds[5] - bounds[4]) * i / numSearchPoints) };

    // test FindClosestPoint
    vtkIdType locator1Pt = locator1->FindClosestPoint(point);
    vtkIdType locator2Pt = locator2->FindClosestPoint(point);

    if (!ArePointsEquidistant(point, locator1Pt, locator2Pt, sgrid))
    {
      return EXIT_FAILURE;
    }

    // test FindClosestNPoints
    int N = 1 + i * 250 / numSearchPoints; // test different amounts of points to search for
    locator1->FindClosestNPoints(N, point, locator1List);
    locator2->FindClosestNPoints(N, point, locator2List);
    if (!ArePointsEquidistant(point, locator1Pt, locator1List->GetId(0), sgrid))
    {
      return EXIT_FAILURE;
    }
    if (!ArePointsEquidistant(point, locator2Pt, locator2List->GetId(0), sgrid))
    {
      return EXIT_FAILURE;
    }

    for (j = 0; j < N; j++)
    {
      if (!ArePointsEquidistant(point, locator1List->GetId(j), locator2List->GetId(j), sgrid))
      {
        return EXIT_FAILURE;
      }
    }

    // test FindPointsWithinRadius
    double radius = 10;
    locator1->FindPointsWithinRadius(radius, point, locator1List);
    locator2->FindPointsWithinRadius(radius, point, locator2List);

    if (!DoesListHaveProperPoints(point, locator1List, locator2List, sgrid) ||
      !DoesListHaveProperPoints(point, locator2List, locator1List, sgrid))
    {
      return EXIT_FAILURE;
    }

    // test FindClosestPointWithinRadius
    double dist2;
    locator1Pt = locator1->FindClosestPointWithinRadius(radius, point, dist2);
    locator2Pt = locator2->FindClosestPointWithinRadius(radius, point, dist2);
    if (locator1Pt < 0 || locator2Pt < 0)
    {
      if (locator1Pt >= 0 || locator2Pt >= 0)
      {
        return EXIT_FAILURE;
      }
    }
    else if (!ArePointsEquidistant(point, locator1Pt, locator2Pt, sgrid))
    {
      return EXIT_FAILURE;
    }
    if (locator1Pt >= 0)
    {
      locator1List->Reset();
      locator1List->InsertNextId(locator1Pt);
      if (!DoesListHaveProperPoints(point, locator1List, locator2List, sgrid))
      {
        return EXIT_FAILURE;
      }
    }
  }

  return EXIT_SUCCESS;
}

int RegressionTestEmbreePointLocator()
{
  int rval = 0;
  int i, j, k, kOffset, jOffset, offset;
  float x[3];
  static const int dims[3] = { 39, 31, 31 };
  vtkNew<vtkTimerLog> tl;

  // number of points to test timing against
  const int numSearchPoints = 50;

  // return value.
  // Positive if Embree averaged faster than uniform and kd tree locators
  // max score of numSearchPoints
  int embreeScore = 0;

  // Create the structured grid.
  vtkNew<vtkStructuredGrid> sgrid;
  sgrid->SetDimensions(dims);

  // We also create the points.
  vtkNew<vtkPoints> points;
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

  // create each point locator type
  vtkNew<vtkKdTreePointLocator> kdTreeLocator;
  vtkNew<vtkPointLocator> uniformLocator;
  vtkNew<vtkEmbreePointLocator> embreeLocator;

  kdTreeLocator->SetDataSet(sgrid);
  kdTreeLocator->BuildLocator();

  uniformLocator->SetDataSet(sgrid);
  uniformLocator->BuildLocator();

  embreeLocator->SetDataSet(sgrid);
  embreeLocator->BuildLocator();

  double bounds[6];
  sgrid->GetBounds(bounds);
  for (i = 0; i < 3; i++)
  {
    // expand the search so we are looking for points inside and outside the BB
    bounds[i * 2] *= .5;
    bounds[i * 2 + 1] *= 1.2;
  }

  vtkNew<vtkIdList> kdTreeList;
  vtkNew<vtkIdList> uniformList;
  vtkNew<vtkIdList> embreeList;

  double kdTreeTime = 0;
  double uniformTime = 0;
  double embreeTime = 0;

  for (i = 0; i < numSearchPoints; i++)
  {
    double point[3] = { (bounds[0] + (bounds[1] - bounds[0]) * i / numSearchPoints),
      (bounds[2] + (bounds[3] - bounds[2]) * i / numSearchPoints),
      (bounds[4] + (bounds[5] - bounds[4]) * i / numSearchPoints) };

    tl->StartTimer();
    vtkIdType kdTreePt = kdTreeLocator->FindClosestPoint(point);
    tl->StopTimer();
    kdTreeTime = tl->GetElapsedTime();

    tl->StartTimer();
    vtkIdType uniformPt = uniformLocator->FindClosestPoint(point);
    tl->StopTimer();
    uniformTime = tl->GetElapsedTime();

    tl->StartTimer();
    vtkIdType embreePt = embreeLocator->FindClosestPoint(point);
    tl->StopTimer();
    embreeTime = tl->GetElapsedTime();

    // check correctness of embree-identified point
    if (!ArePointsEquidistant(point, embreePt, kdTreePt, sgrid))
    {
      return EXIT_FAILURE;
    }
    if (!ArePointsEquidistant(point, embreePt, uniformPt, sgrid))
    {
      return EXIT_FAILURE;
    }

    // check search time of embree compared to kd tree and uniform search
    if (embreeTime < kdTreeTime && embreeTime < uniformTime)
    {
      embreeScore++;
    }
    else
    {
      embreeScore--;
    }
  }
  // positive score -> embree is faster on average
  // negative score -> embree is slower on average
  return embreeScore;
}

int TestEmbreePointLocator(int, char*[])
{
  int rval = 0;

  vtkNew<vtkKdTreePointLocator> kdTreeLocator;
  vtkNew<vtkEmbreePointLocator> embreeLocator;

  // offers a correctness test for embree against known locator implementation
  rval += ComparePointLocators(embreeLocator, "embree", kdTreeLocator, "kdtree");

  // offers a performance test for embree vs kd tree and uniform locators
  int embreeScore = RegressionTestEmbreePointLocator();
  rval += (embreeScore > 0) ? 0 : 1;

  return rval;
}
