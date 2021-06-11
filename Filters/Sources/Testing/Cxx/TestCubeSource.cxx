// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkCubeSource.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkSmartPointer.h>

int TestCubeSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkSmartPointer<vtkMinimalStandardRandomSequence> randomSequence =
    vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  randomSequence->SetSeed(1);

  vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();

  cubeSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  double center[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  cubeSource->SetCenter(center);

  double xLength = randomSequence->GetNextValue();
  cubeSource->SetXLength(xLength);

  double yLength = randomSequence->GetNextValue();
  cubeSource->SetYLength(yLength);

  double zLength = randomSequence->GetNextValue();
  cubeSource->SetZLength(zLength);

  cubeSource->Update();

  vtkSmartPointer<vtkPolyData> polyData = cubeSource->GetOutput();
  vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

  if (points->GetDataType() != VTK_FLOAT)
  {
    return EXIT_FAILURE;
  }

  cubeSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  cubeSource->SetCenter(center);

  xLength = randomSequence->GetNextValue();
  cubeSource->SetXLength(xLength);

  yLength = randomSequence->GetNextValue();
  cubeSource->SetYLength(yLength);

  zLength = randomSequence->GetNextValue();
  cubeSource->SetZLength(zLength);

  cubeSource->Update();

  polyData = cubeSource->GetOutput();
  points = polyData->GetPoints();

  if (points->GetDataType() != VTK_DOUBLE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
