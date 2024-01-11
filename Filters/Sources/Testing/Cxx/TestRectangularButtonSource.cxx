// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkMinimalStandardRandomSequence.h>
#include <vtkRectangularButtonSource.h>
#include <vtkSmartPointer.h>

int TestRectangularButtonSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkSmartPointer<vtkMinimalStandardRandomSequence> randomSequence =
    vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  randomSequence->SetSeed(1);

  vtkSmartPointer<vtkRectangularButtonSource> rectangularButtonSource =
    vtkSmartPointer<vtkRectangularButtonSource>::New();
  rectangularButtonSource->SetBoxRatio(1.0);
  rectangularButtonSource->SetTextureHeightRatio(1.0);
  rectangularButtonSource->SetTextureRatio(1.0);
  rectangularButtonSource->SetShoulderTextureCoordinate(0.0, 0.0);
  rectangularButtonSource->SetTextureDimensions(100, 100);
  rectangularButtonSource->SetTextureStyleToProportional();
  rectangularButtonSource->TwoSidedOff();

  rectangularButtonSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  double center[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  rectangularButtonSource->SetCenter(center);

  double depth = randomSequence->GetNextValue();
  rectangularButtonSource->SetDepth(depth);

  double height = randomSequence->GetNextValue();
  rectangularButtonSource->SetHeight(height);

  double width = randomSequence->GetNextValue();
  rectangularButtonSource->SetWidth(width);

  rectangularButtonSource->Update();

  vtkSmartPointer<vtkPolyData> polyData = rectangularButtonSource->GetOutput();
  vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

  if (points->GetDataType() != VTK_FLOAT)
  {
    return EXIT_FAILURE;
  }

  rectangularButtonSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  rectangularButtonSource->SetCenter(center);

  depth = randomSequence->GetNextValue();
  rectangularButtonSource->SetDepth(depth);

  height = randomSequence->GetNextValue();
  rectangularButtonSource->SetHeight(height);

  width = randomSequence->GetNextValue();
  rectangularButtonSource->SetWidth(width);

  rectangularButtonSource->Update();

  polyData = rectangularButtonSource->GetOutput();
  points = polyData->GetPoints();

  if (points->GetDataType() != VTK_DOUBLE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
