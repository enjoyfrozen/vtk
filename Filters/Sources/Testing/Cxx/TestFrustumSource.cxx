// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkCamera.h>
#include <vtkFrustumSource.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkPlanes.h>
#include <vtkSmartPointer.h>

int TestFrustumSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkSmartPointer<vtkMinimalStandardRandomSequence> randomSequence =
    vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  randomSequence->SetSeed(1);

  vtkSmartPointer<vtkFrustumSource> frustumSource = vtkSmartPointer<vtkFrustumSource>::New();
  frustumSource->ShowLinesOn();

  frustumSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  double linesLength = randomSequence->GetNextValue();
  frustumSource->SetLinesLength(linesLength);

  vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();

  double position[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    position[i] = randomSequence->GetNextValue();
  }
  camera->SetPosition(position);
  double focalPoint[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    focalPoint[i] = randomSequence->GetNextValue();
  }
  camera->SetFocalPoint(focalPoint);
  double planeCoefficients[24];
  camera->GetFrustumPlanes(1.0, planeCoefficients);

  vtkSmartPointer<vtkPlanes> planes = vtkSmartPointer<vtkPlanes>::New();
  planes->SetFrustumPlanes(planeCoefficients);
  frustumSource->SetPlanes(planes);

  frustumSource->Update();

  vtkSmartPointer<vtkPolyData> polyData = frustumSource->GetOutput();
  vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

  if (points->GetDataType() != VTK_FLOAT)
  {
    return EXIT_FAILURE;
  }

  frustumSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  linesLength = randomSequence->GetNextValue();
  frustumSource->SetLinesLength(linesLength);

  for (unsigned int i = 0; i < 3; ++i)
  {
    position[i] = randomSequence->GetNextValue();
  }
  camera->SetPosition(position);
  for (unsigned int i = 0; i < 3; ++i)
  {
    focalPoint[i] = randomSequence->GetNextValue();
  }
  camera->SetFocalPoint(focalPoint);
  camera->GetFrustumPlanes(1.0, planeCoefficients);

  planes->SetFrustumPlanes(planeCoefficients);
  frustumSource->SetPlanes(planes);

  frustumSource->Update();

  polyData = frustumSource->GetOutput();
  points = polyData->GetPoints();

  if (points->GetDataType() != VTK_DOUBLE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
