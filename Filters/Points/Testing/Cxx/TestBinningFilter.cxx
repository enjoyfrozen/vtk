/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestBinningFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// This test covers the vtkBinningFilter

#include "vtkBinningFilter.h"

#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSetGet.h"
#include "vtkTestUtilities.h"
#include "vtkXMLPolyDataReader.h"

int TestArray(vtkDataArray* array, double expectedMin, double expectedMax)
{
  constexpr double epsilon = 0.01;
  double range[2] = { 1, -1 };
  // compare magnitude
  array->GetRange(range, -1);
  if (std::abs(range[0] - expectedMin) > epsilon || std::abs(range[1] - expectedMax) > epsilon)
  {
    vtkErrorWithObjectMacro(nullptr, "Wrong range for array " << array->GetName());
    return 1;
  }

  return 0;
}

int TestBinningFilter(int argc, char* argv[])
{
  vtkNew<vtkXMLPolyDataReader> reader;
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/particles.vtp");
  reader->SetFileName(fname);
  delete[] fname;

  reader->Update();
  vtkPolyData* input = reader->GetOutput();
  vtkPointData* inPointData = input->GetPointData();

  vtkNew<vtkBinningFilter> binFilter;
  binFilter->SetInputConnection(reader->GetOutputPort());
  binFilter->SetDimensions(2, 2, 2);
  binFilter->Update();

  vtkImageData* output = binFilter->GetOutput();
  vtkCellData* outCellData = output->GetCellData();

  if (outCellData->GetNumberOfArrays() != inPointData->GetNumberOfArrays() + 1)
  {
    vtkErrorWithObjectMacro(nullptr, "Wrong number of arrays in output");
    return EXIT_FAILURE;
  }

  int errors = 0;
  vtkDataArray* outArray = outCellData->GetArray("NumberOfParticles");
  errors += TestArray(outArray, 1, 7);
  outArray = outCellData->GetArray("RandomPointScalars");
  errors += TestArray(outArray, 9.686, 58.627);
  outArray = outCellData->GetArray("RandomPointVectors");
  errors += TestArray(outArray, 71.199, 121.372);

  return EXIT_SUCCESS;
}
