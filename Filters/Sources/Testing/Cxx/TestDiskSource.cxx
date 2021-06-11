// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkDiskSource.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkSmartPointer.h>

int TestDiskSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkSmartPointer<vtkMinimalStandardRandomSequence> randomSequence =
    vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  randomSequence->SetSeed(1);

  vtkSmartPointer<vtkDiskSource> diskSource = vtkSmartPointer<vtkDiskSource>::New();
  diskSource->SetCircumferentialResolution(8);
  diskSource->SetRadialResolution(8);

  diskSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  double innerRadius = randomSequence->GetNextValue();

  double outerRadius = randomSequence->GetNextValue();

  if (innerRadius > outerRadius)
  {
    std::swap(innerRadius, outerRadius);
  }

  diskSource->SetInnerRadius(innerRadius);
  diskSource->SetOuterRadius(outerRadius);

  diskSource->Update();

  vtkSmartPointer<vtkPolyData> polyData = diskSource->GetOutput();
  vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

  if (points->GetDataType() != VTK_FLOAT)
  {
    return EXIT_FAILURE;
  }

  diskSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  innerRadius = randomSequence->GetNextValue();

  outerRadius = randomSequence->GetNextValue();

  if (innerRadius > outerRadius)
  {
    std::swap(innerRadius, outerRadius);
  }

  diskSource->SetInnerRadius(innerRadius);
  diskSource->SetOuterRadius(outerRadius);

  diskSource->Update();

  polyData = diskSource->GetOutput();
  points = polyData->GetPoints();

  if (points->GetDataType() != VTK_DOUBLE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
