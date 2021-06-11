// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkConeSource.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkSmartPointer.h>

int TestConeSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkSmartPointer<vtkMinimalStandardRandomSequence> randomSequence =
    vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  randomSequence->SetSeed(1);

  vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
  coneSource->SetResolution(8);
  coneSource->CappingOn();

  coneSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  double center[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  coneSource->SetCenter(center);

  double direction[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    direction[i] = randomSequence->GetNextValue();
  }
  coneSource->SetDirection(direction);

  double height = randomSequence->GetNextValue();
  coneSource->SetHeight(height);

  double radius = randomSequence->GetNextValue();
  coneSource->SetRadius(radius);

  coneSource->Update();

  vtkSmartPointer<vtkPolyData> polyData = coneSource->GetOutput();
  vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

  if (points->GetDataType() != VTK_FLOAT)
  {
    return EXIT_FAILURE;
  }

  coneSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  coneSource->SetCenter(center);

  for (unsigned int i = 0; i < 3; ++i)
  {
    direction[i] = randomSequence->GetNextValue();
  }
  coneSource->SetDirection(direction);

  height = randomSequence->GetNextValue();
  coneSource->SetHeight(height);

  radius = randomSequence->GetNextValue();
  coneSource->SetRadius(radius);

  coneSource->Update();

  polyData = coneSource->GetOutput();
  points = polyData->GetPoints();

  if (points->GetDataType() != VTK_DOUBLE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
