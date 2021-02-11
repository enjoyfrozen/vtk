/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCameraPathRepresentation.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "WidgetTestingMacros.h"
#include "vtkCamera.h"
#include "vtkCameraPathRepresentation.h"
#include "vtkDoubleArray.h"
#include "vtkParametricSpline.h"
#include "vtkTestErrorObserver.h"

#include <cstdlib>
#include <iostream>

int vtkCameraPathRepresentationTest1(int, char*[])
{
  vtkNew<vtkCameraPathRepresentation> cameraPathRep;

  vtkNew<vtkTest::ErrorObserver> errorObserver;
  cameraPathRep->AddObserver(vtkCommand::ErrorEvent, errorObserver);

  EXERCISE_BASIC_REPRESENTATION_METHODS(vtkCameraPathRepresentation, cameraPathRep);

  TEST_SET_GET_BOOLEAN(cameraPathRep, Directional);

  // clamped 0-3
  TEST_SET_GET_INT_RANGE(cameraPathRep, ProjectionNormal, 1, 2);
  cameraPathRep->SetProjectionNormalToXAxes();
  cameraPathRep->SetProjectionNormalToYAxes();
  cameraPathRep->SetProjectionNormalToZAxes();
  cameraPathRep->SetProjectionNormalToOblique();

  TEST_SET_GET_DOUBLE_RANGE(cameraPathRep, ProjectionPosition, -10.0, 10.0);

  vtkNew<vtkPolyData> pd;
  cameraPathRep->GetPolyData(pd);
  if (pd->GetNumberOfPoints() == 0)
  {
    std::cerr << "Error: Polydata is empty" << std::endl;
  }
  vtkSmartPointer<vtkProperty> prop = cameraPathRep->GetHandleProperty();
  if (prop == nullptr)
  {
    std::cerr << "Error: Handle Property is nullptr." << std::endl;
  }
  prop = cameraPathRep->GetSelectedHandleProperty();
  if (prop == nullptr)
  {
    std::cerr << "Error: Selected Handle Property is nullptr." << std::endl;
  }
  prop = cameraPathRep->GetLineProperty();
  if (prop == nullptr)
  {
    std::cerr << "Error: Line Property is nullptr." << std::endl;
  }
  prop = cameraPathRep->GetSelectedLineProperty();
  if (prop == nullptr)
  {
    std::cerr << "Error: Selected Line Property is nullptr." << std::endl;
  }

  cameraPathRep->SetNumberOfHandles(10);
  int numHandles = cameraPathRep->GetNumberOfHandles();
  if (numHandles != 10)
  {
    std::cerr << "Error in Setting number of Handles to 10, got " << numHandles << std::endl;
    return EXIT_FAILURE;
  }

  cameraPathRep->SetNumberOfHandles(-1);
  errorObserver->CheckErrorMessage("Cannot set a negative number of handles.");
  numHandles = cameraPathRep->GetNumberOfHandles();
  if (numHandles != 10)
  {
    std::cerr << "Error: setting NumberOfHandle to -1 should fail without modification"
              << numHandles << std::endl;
  }

  vtkNew<vtkParametricSpline> pspline;
  cameraPathRep->SetNumberOfHandles(10);
  pspline->SetPoints(cameraPathRep->GetParametricSpline()->GetPoints());
  cameraPathRep->SetParametricSpline(pspline);
  vtkSmartPointer<vtkParametricSpline> pspline2 = cameraPathRep->GetParametricSpline();
  if (pspline2.GetPointer() != pspline.GetPointer())
  {
    std::cerr << "Error setting/getting parametric spline." << std::endl;
    return EXIT_FAILURE;
  }

  numHandles = 5;
  cameraPathRep->SetNumberOfHandles(numHandles);
  double x, y, z;
  x = y = z = 0.0;
  double xyz[3] = { 0.0, 0.0, 0.0 };
  double* hpos;
  double hpos2[3];
  for (int h = 0; h < numHandles; h++)
  {
    cameraPathRep->SetHandlePosition(h, x, y, z);
    hpos = cameraPathRep->GetHandlePosition(h);
    if (!hpos)
    {
      std::cerr << "Null handle position back for handle " << h << std::endl;
      return EXIT_FAILURE;
    }
    else if (hpos[0] != x || hpos[1] != y || hpos[2] != z)
    {
      std::cerr << "Failure in SetHandlePosition(" << h << "," << x << "," << y << "," << z
                << "), got " << hpos[0] << ", " << hpos[1] << ", " << hpos[2] << std::endl;
      return EXIT_FAILURE;
    }
    cameraPathRep->GetHandlePosition(h, hpos2);
    if (hpos2[0] != x || hpos2[1] != y || hpos2[2] != z)
    {
      std::cerr << "Failure in SetHandlePosition(" << h << "," << x << "," << y << "," << z
                << "), got " << hpos2[0] << ", " << hpos2[1] << ", " << hpos2[2] << std::endl;
      return EXIT_FAILURE;
    }

    cameraPathRep->SetHandlePosition(h, xyz);
    hpos = cameraPathRep->GetHandlePosition(h);
    if (!hpos)
    {
      std::cerr << "Null handle position back for handle " << h << std::endl;
      return EXIT_FAILURE;
    }
    else if (hpos[0] != xyz[0] || hpos[1] != xyz[1] || hpos[2] != xyz[2])
    {
      std::cerr << "Failure in SetHandlePosition(" << h << ", xyz), expected " << xyz[0] << ", "
                << xyz[1] << ", " << xyz[2] << ", got " << hpos[0] << ", " << hpos[1] << ", "
                << hpos[2] << std::endl;
      return EXIT_FAILURE;
    }
    cameraPathRep->GetHandlePosition(h, hpos2);
    if (hpos2[0] != xyz[0] || hpos2[1] != xyz[1] || hpos2[2] != xyz[2])
    {
      std::cerr << "Failure in SetHandlePosition(" << h << ",xyz), , expected " << xyz[0] << ", "
                << xyz[1] << ", " << xyz[2] << ", got " << hpos2[0] << ", " << hpos2[1] << ", "
                << hpos2[2] << std::endl;
      return EXIT_FAILURE;
    }
    x -= 1.0;
    y += 1.0;
    z += 2.5;
    xyz[0] += 1.0;
    xyz[1] -= 1.0;
    xyz[2] += 3.9;
  }
  vtkSmartPointer<vtkDoubleArray> da = cameraPathRep->GetHandlePositions();
  if (da == nullptr)
  {
    std::cerr << "HandlePositions array is null!" << std::endl;
    return EXIT_FAILURE;
  }
  else
  {
    for (vtkIdType i = 0; i < da->GetNumberOfTuples(); i++)
    {
      double val[3];
      da->GetTypedTuple(i, val);
    }
  }

  cameraPathRep->SetNumberOfHandles(0);
  if (cameraPathRep->GetNumberOfHandles() != 0)
  {
    std::cerr << "Fail to reset number of handle" << std::endl;
    return EXIT_FAILURE;
  }

  numHandles = 5;
  x = y = z = 0.0;
  xyz[0] = xyz[1] = xyz[2] = 0.0;
  for (int h = 0; h < numHandles; h++)
  {
    vtkNew<vtkCamera> cam;
    cam->SetPosition(x, y, z);
    cameraPathRep->AddCameraAt(cam, h);
    hpos = cameraPathRep->GetHandlePosition(h);
    if (!hpos)
    {
      std::cerr << "Null handle position back for handle " << h << std::endl;
      return EXIT_FAILURE;
    }
    else if (hpos[0] != x || hpos[1] != y || hpos[2] != z)
    {
      std::cerr << "Failure in SetHandlePosition(" << h << "," << x << "," << y << "," << z
                << "), got " << hpos[0] << ", " << hpos[1] << ", " << hpos[2] << std::endl;
      return EXIT_FAILURE;
    }
    cameraPathRep->GetHandlePosition(h, hpos2);
    if (hpos2[0] != x || hpos2[1] != y || hpos2[2] != z)
    {
      std::cerr << "Failure in SetHandlePosition(" << h << "," << x << "," << y << "," << z
                << "), got " << hpos2[0] << ", " << hpos2[1] << ", " << hpos2[2] << std::endl;
      return EXIT_FAILURE;
    }

    cameraPathRep->SetHandlePosition(h, xyz);
    hpos = cameraPathRep->GetHandlePosition(h);
    if (!hpos)
    {
      std::cerr << "Null handle position back for handle " << h << std::endl;
      return EXIT_FAILURE;
    }
    else if (hpos[0] != xyz[0] || hpos[1] != xyz[1] || hpos[2] != xyz[2])
    {
      std::cerr << "Failure in SetHandlePosition(" << h << ", xyz), expected " << xyz[0] << ", "
                << xyz[1] << ", " << xyz[2] << ", got " << hpos[0] << ", " << hpos[1] << ", "
                << hpos[2] << std::endl;
      return EXIT_FAILURE;
    }
    cameraPathRep->GetHandlePosition(h, hpos2);
    if (hpos2[0] != xyz[0] || hpos2[1] != xyz[1] || hpos2[2] != xyz[2])
    {
      std::cerr << "Failure in SetHandlePosition(" << h << ",xyz), , expected " << xyz[0] << ", "
                << xyz[1] << ", " << xyz[2] << ", got " << hpos2[0] << ", " << hpos2[1] << ", "
                << hpos2[2] << std::endl;
      return EXIT_FAILURE;
    }
    x -= 1.0;
    y += 1.0;
    z += 2.5;
    xyz[0] += 1.0;
    xyz[1] -= 1.0;
    xyz[2] += 3.9;
  }
  da = cameraPathRep->GetHandlePositions();
  if (da == nullptr)
  {
    std::cerr << "HandlePositions array is null!" << std::endl;
    return EXIT_FAILURE;
  }
  else
  {
    for (vtkIdType i = 0; i < da->GetNumberOfTuples(); i++)
    {
      double val[3];
      da->GetTypedTuple(i, val);
    }
  }

  // 0 is invalid
  TEST_SET_GET_INT_RANGE(cameraPathRep, Resolution, 10, 100);

  TEST_SET_GET_BOOLEAN(cameraPathRep, Closed);

  cameraPathRep->DeleteCameraAt(2);

  numHandles = cameraPathRep->GetNumberOfHandles();

  if (numHandles != 4)
  {
    std::cerr << "Error with deleting a camera " << numHandles << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(2);
  points->SetPoint(0, 3.0, 6.8, -9.9);
  points->SetPoint(1, -3.0, -6.8, 9.9);
  cameraPathRep->InitializeHandles(points);
  da = cameraPathRep->GetHandlePositions();
  if (da == nullptr)
  {
    std::cerr << "HandlePositions array is null after initing with vtkPoints!" << std::endl;
    return EXIT_FAILURE;
  }
  else
  {
    for (vtkIdType i = 0; i < da->GetNumberOfTuples(); i++)
    {
      double val[3];
      da->GetTypedTuple(i, val);
    }
  }

  cameraPathRep->SetLineColor(1.0, 0.5, 0.3);

  cameraPathRep->SetCurrentHandleFocalPoint(13, 37, 0);
  if (cameraPathRep->GetLastModifiedCamera() != 1)
  {
    std::cerr << "Error: setting focal point did not set LastModifiedCamera" << std::endl;
  }
  double fp[3];
  cameraPathRep->GetHandleFocalPoint(1, fp);
  if (fp[0] != 13)
  {
    std::cerr << "Failed to set current handle focal point" << std::endl;
  }

  cameraPathRep->SetHandlePosition(0, 13, 37, 0);
  cameraPathRep->SetCurrentHandleIndex(0);

  auto point = cameraPathRep->GetCurrentHandlePosition();
  if (point[0] != 13)
  {
    std::cerr << "Failed to set current handle position" << std::endl;
  }

  return EXIT_SUCCESS;
}
