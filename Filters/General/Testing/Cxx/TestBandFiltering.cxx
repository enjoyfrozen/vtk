/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestBandFiltering.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <vtkBandFiltering.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkMathUtilities.h>
#include <vtkNew.h>
#include <vtkTable.h>
#include <vtkTableFFT.h>

#include <array>

namespace
{
constexpr std::array<double, 6> EXPECTED_VALUE1 = { 0.326591, 0.340033, 0.407533, 0.46924, 1.15643,
  4.9792 };
constexpr std::array<double, 20> EXPECTED_VALUE2 = { 89.5688, 68.191, 89.5924, 70.4061, 81.0587,
  89.6872, 80.003, 85.7241, 82.8791, 86.8522, 87.555, 89.3785, 92.708, 98.1622, 111.79, 96.5032,
  87.3854 };

//----------------------------------------------------------------------------
void InitializeTableInput(vtkTable* table, int numberOfPoint = 20, float maxRange = 10)
{
  vtkNew<vtkDoubleArray> pa;
  pa->SetName("Pa");
  pa->SetNumberOfTuples(numberOfPoint);
  pa->SetNumberOfComponents(1);

  for (vtkIdType i = 0; i < numberOfPoint; i++)
  {
    pa->InsertNextTuple1(sin(2 * i) + sin(vtkMath::Pi() * i));
  }

  // Fill input
  table->AddColumn(pa);
}
}

// ----------------------------------------------------------------------------
int TestBandFiltering(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  // Fill our data
  vtkNew<vtkTable> input;
  ::InitializeTableInput(input, 100, 10);

  // Testing Octave band filtering
  vtkNew<vtkBandFiltering> bandFiltering;
  bandFiltering->SetInputData(input);
  bandFiltering->SetOutputInDecibel(false);
  bandFiltering->SetBandFilteringMode(vtkBandFiltering::OCTAVE);
  bandFiltering->Update();

  vtkDoubleArray* arr =
    vtkDoubleArray::SafeDownCast(bandFiltering->GetOutput()->GetColumnByName("Pa"));

  if (!arr)
  {
    return 1;
  }

  if (arr->GetNumberOfValues() != 6)
  {
    std::cerr << "Wrong number of values. Expected 6 but got " << arr->GetNumberOfValues()
              << std::endl;
    return 1;
  }

  for (vtkIdType i = 0; i < arr->GetNumberOfValues(); i++)
  {
    if (!vtkMathUtilities::NearlyEqual(arr->GetValue(i), ::EXPECTED_VALUE1[i], 1.0e-06))
    {
      std::cerr << "Wrong value in Pa for the octave band filtering. Expected "
                << ::EXPECTED_VALUE1[i] << " but got " << arr->GetValue(i) << std::endl;
      return 1;
    }
  }

  // Testing Third Octave filtering with an fft input
  vtkNew<vtkTableFFT> tableFFT;
  tableFFT->SetInputData(input);
  tableFFT->CreateFrequencyColumnOn();
  tableFFT->Update();

  bandFiltering->SetInputData(tableFFT->GetOutput());
  bandFiltering->SetProcessedColumnName("Pa");
  bandFiltering->SetApplyFFT(false);
  bandFiltering->SetOutputInDecibel(true);
  bandFiltering->SetBandFilteringMode(vtkBandFiltering::THIRD_OCTAVE);
  bandFiltering->Update();

  arr = vtkDoubleArray::SafeDownCast(bandFiltering->GetOutput()->GetColumnByName("Decibel (dB)"));

  if (!arr)
  {
    return 1;
  }

  if (arr->GetNumberOfValues() != 20)
  {
    std::cerr << "Wrong number of values. Expected 20 but got " << arr->GetNumberOfValues()
              << std::endl;
    return 1;
  }

  for (vtkIdType i = 0; i < arr->GetNumberOfValues(); i++)
  {
    if (!vtkMathUtilities::NearlyEqual(arr->GetValue(i), ::EXPECTED_VALUE2[i], 1.0e-05))
    {
      std::cerr << "Wrong value in Db for the third octave band filtering. Expected "
                << ::EXPECTED_VALUE2[i] << " but got " << arr->GetValue(i) << std::endl;
    }
  }

  return 0;
}
