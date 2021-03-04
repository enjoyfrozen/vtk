/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestXMLWriteRead.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkFloatArray.h"
#include "vtkHDFReader.h"
#include "vtkImageData.h"
#include "vtkMathUtilities.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkTesting.h"

#include <iterator>
#include <string>

std::array<float, 100> iterationsExpected = { 1.86486, 1.95653, 2.07518, 2.25662, 2.50157, 2.79127,
  3.00551, 3.02831, 3.05835, 3.09675, 3.14016, 3.1689, 3.1368, 3.03256, 2.88091, 2.71876, 2.57046,
  2.44094, 2.33208, 2.24341, 1.92419, 2.04571, 2.2997, 2.73842, 3.0154, 3.05095, 3.10242, 3.18099,
  3.31121, 3.55651, 4.01317, 4.31894, 6.25067, 4.47795, 3.97399, 3.12661, 2.89133, 2.67115, 2.49467,
  2.35697, 1.98462, 2.27505, 3.00099, 3.03909, 3.09313, 3.17153, 3.29262, 3.49999, 3.92148, 4.10829,
  4.50946, 5.39993, 9.68122, 11.2083, 4.63469, 4.07338, 3.29798, 3.00397, 2.72288, 2.50939, 2.13683,
  3.00452, 3.05639, 3.12907, 3.23069, 3.38032, 3.62259, 4.01278, 4.18039, 4.56284, 5.20768, 7.10931,
  14.029, 10.8399, 5.63, 4.74784, 4.07164, 3.26951, 3.01224, 2.71134, 2.82759, 3.0641, 3.15926,
  3.28517, 3.45558, 3.70619, 4.03985, 4.30976, 4.80899, 6.08468, 6.95028, 9.04998, 100, 100,
  7.46957, 6.00287, 5.09513, 3.84623, 3.16721, 2.97355 };

bool FuzzyEqual(float a, float b)
{
  return vtkMathUtilities::NearlyEqual(a, b, 20 * std::numeric_limits<float>::epsilon());
}

int TestHDFReader(int argc, char* argv[])
{
  vtkNew<vtkTesting> testHelper;
  testHelper->AddArguments(argc, argv);
  if (!testHelper->IsFlagSpecified("-D"))
  {
    std::cerr << "Error: -D /path/to/data was not specified.";
    return EXIT_FAILURE;
  }

  std::string dataRoot = testHelper->GetDataRoot();
  // ImageData file
  // ------------------------------------------------------------
  std::string fileName = dataRoot + "/Data/mandelbrot-vti.hdf";
  vtkNew<vtkHDFReader> reader;
  if (!reader->CanReadFile(fileName.c_str()))
  {
    return EXIT_FAILURE;
  }
  reader->SetFileName(fileName.c_str());
  reader->Update();
  vtkImageData* data = vtkImageData::SafeDownCast(reader->GetOutput());
  if (data == nullptr)
  {
    std::cerr << "Error: not vtkImageData" << std::endl;
    return EXIT_FAILURE;
  }
  int* dims = data->GetDimensions();
  if (dims[0] != 20 || dims[1] != 21 || dims[2] != 22)
  {
    std::cerr << "Error: vtkImageData with wrong dimensions: "
              << "expecting [19, 20, 21] got  [" << dims[0] << ", " << dims[1] << ", " << dims[2]
              << "]" << std::endl;
    return EXIT_FAILURE;
  }
  vtkDataArray* da = data->GetPointData()->GetArray("Iterations");
  vtkFloatArray* fa = vtkFloatArray::SafeDownCast(da);
  if (!fa)
  {
    std::cerr << "Error: No Iterations array" << std::endl;
    return EXIT_FAILURE;
  }
  float* iterationsData = fa->GetPointer(0);
  for (auto itE = iterationsExpected.begin(), itD = iterationsData; itE != iterationsExpected.end();
       ++itE, ++itD)
  {
    if (!vtkMathUtilities::NearlyEqual(*itE, *itD, 100 * std::numeric_limits<float>::epsilon()))
    {
      std::cerr << "Error: Data in Iterations array does not match expected data: " << *itD << " "
                << *itE << std::endl;
      ;

      return EXIT_FAILURE;
    }
  }

  // UnstructuredGrid file
  //------------------------------------------------------------
  fileName = dataRoot + "/Data/can-vtu.hdf";
  if (!reader->CanReadFile(fileName.c_str()))
  {
    return EXIT_FAILURE;
  }
  reader->SetFileName(fileName.c_str());
  reader->Update();
  return EXIT_SUCCESS;
}
