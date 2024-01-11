// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkMinimalStandardRandomSequence.h>
#include <vtkParametricEllipsoid.h>
#include <vtkParametricFunctionSource.h>
#include <vtkSmartPointer.h>

int TestParametricFunctionSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkSmartPointer<vtkMinimalStandardRandomSequence> randomSequence =
    vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
  randomSequence->SetSeed(1);

  vtkSmartPointer<vtkParametricFunctionSource> parametricFunctionSource =
    vtkSmartPointer<vtkParametricFunctionSource>::New();
  parametricFunctionSource->SetUResolution(64);
  parametricFunctionSource->SetVResolution(64);
  parametricFunctionSource->SetWResolution(64);
  parametricFunctionSource->SetScalarModeToNone();
  parametricFunctionSource->GenerateTextureCoordinatesOff();

  parametricFunctionSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  vtkSmartPointer<vtkParametricEllipsoid> parametricEllipsoid =
    vtkSmartPointer<vtkParametricEllipsoid>::New();

  double xRadius = randomSequence->GetNextValue();
  parametricEllipsoid->SetXRadius(xRadius);

  double yRadius = randomSequence->GetNextValue();
  parametricEllipsoid->SetYRadius(yRadius);

  double zRadius = randomSequence->GetNextValue();
  parametricEllipsoid->SetZRadius(zRadius);

  parametricFunctionSource->SetParametricFunction(parametricEllipsoid);

  parametricFunctionSource->Update();

  vtkSmartPointer<vtkPolyData> polyData = parametricFunctionSource->GetOutput();
  vtkSmartPointer<vtkPoints> points = polyData->GetPoints();

  if (points->GetDataType() != VTK_FLOAT)
  {
    return EXIT_FAILURE;
  }

  parametricFunctionSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);

  xRadius = randomSequence->GetNextValue();
  parametricEllipsoid->SetXRadius(xRadius);

  yRadius = randomSequence->GetNextValue();
  parametricEllipsoid->SetYRadius(yRadius);

  zRadius = randomSequence->GetNextValue();
  parametricEllipsoid->SetZRadius(zRadius);

  parametricFunctionSource->SetParametricFunction(parametricEllipsoid);

  parametricFunctionSource->Update();

  polyData = parametricFunctionSource->GetOutput();
  points = polyData->GetPoints();

  if (points->GetDataType() != VTK_DOUBLE)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
