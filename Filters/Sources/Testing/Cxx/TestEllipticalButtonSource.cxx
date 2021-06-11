// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkEllipticalButtonSource.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkSmartPointer.h>

int TestEllipticalButtonSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkSmartPointer<vtkMinimalStandardRandomSequence> randomSequence =
    vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  randomSequence->SetSeed(1);

  vtkSmartPointer<vtkEllipticalButtonSource> ellipticalButtonSource =
    vtkSmartPointer<vtkEllipticalButtonSource>::New();
  ellipticalButtonSource->SetCircumferentialResolution(8);
  ellipticalButtonSource->SetShoulderResolution(8);
  ellipticalButtonSource->SetTextureResolution(8);
  ellipticalButtonSource->SetRadialRatio(1.0);
  ellipticalButtonSource->SetShoulderTextureCoordinate(0.0, 0.0);
  ellipticalButtonSource->SetTextureDimensions(100, 100);
  ellipticalButtonSource->SetTextureStyleToProportional();
  ellipticalButtonSource->TwoSidedOff();

  ellipticalButtonSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  double center[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  ellipticalButtonSource->SetCenter(center);

  double depth = randomSequence->GetNextValue();
  ellipticalButtonSource->SetDepth(depth);

  double height = randomSequence->GetNextValue();
  ellipticalButtonSource->SetHeight(height);

  double width = randomSequence->GetNextValue();
  ellipticalButtonSource->SetWidth(width);

  ellipticalButtonSource->Update();

  vtkSmartPointer<vtkPolyData> polyData = ellipticalButtonSource->GetOutput();
  vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

  if (points->GetDataType() != VTK_FLOAT)
  {
    return EXIT_FAILURE;
  }

  ellipticalButtonSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  for (unsigned int i = 0; i < 3; ++i)
  {
    center[i] = randomSequence->GetNextValue();
  }
  ellipticalButtonSource->SetCenter(center);

  depth = randomSequence->GetNextValue();
  ellipticalButtonSource->SetDepth(depth);

  height = randomSequence->GetNextValue();
  ellipticalButtonSource->SetHeight(height);

  width = randomSequence->GetNextValue();
  ellipticalButtonSource->SetWidth(width);

  ellipticalButtonSource->Update();

  polyData = ellipticalButtonSource->GetOutput();
  points = polyData->GetPoints();

  if (points->GetDataType() != VTK_DOUBLE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
