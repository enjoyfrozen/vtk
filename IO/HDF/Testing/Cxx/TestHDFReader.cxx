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
#include "vtkXMLImageDataReader.h"

#include <iterator>
#include <string>

bool FuzzyEqual(float a, float b)
{
  return vtkMathUtilities::NearlyEqual(a, b, 20 * std::numeric_limits<float>::epsilon());
}

vtkSmartPointer<vtkImageData> ReadImageData(const std::string& fileName)
{
  vtkNew<vtkXMLImageDataReader> reader;
  reader->SetFileName(fileName.c_str());
  reader->Update();
  vtkSmartPointer<vtkImageData> data = vtkImageData::SafeDownCast(reader->GetOutput());
  return data;
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
  std::cout << "Testing: " << fileName << std::endl;
  vtkNew<vtkHDFReader> reader;
  if (!reader->CanReadFile(fileName.c_str()))
  {
    return EXIT_FAILURE;
  }
  reader->SetFileName(fileName.c_str());
  reader->Update();
  vtkImageData* data = vtkImageData::SafeDownCast(reader->GetOutput());
  vtkSmartPointer<vtkImageData> original = ReadImageData(dataRoot + "/Data/mandelbrot.vti");
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
  vtkFloatArray* df = vtkFloatArray::SafeDownCast(data->GetPointData()->GetArray("Iterations"));
  if (!df)
  {
    std::cerr << "Error: No Iterations array" << std::endl;
    return EXIT_FAILURE;
  }
  float* iterationsData = df->GetPointer(0);
  vtkFloatArray* of = vtkFloatArray::SafeDownCast(original->GetPointData()->GetArray("Iterations"));
  float* iterationsExpected = of->GetPointer(0);
  for (auto itE = iterationsExpected, itD = iterationsData; itE != iterationsExpected + 100;
       ++itE, ++itD)
  {
    if (*itE != *itD)
    {
      std::cerr << "Error: Data in Iterations array does not match expected data: " << *itD << " "
                << *itE << std::endl;
      ;

      return EXIT_FAILURE;
    }
  }

  // UnstructuredGrid file
  //------------------------------------------------------------
  std::vector<std::string> fileNames = { dataRoot + "/Data/can-vtu.hdf",
    dataRoot + "/Data/can-pvtu.hdf" };
  std::vector<std::string> originalNames = { dataRoot + "/Data/can.vtu",
    dataRoot + "/Data/can.pvtu" };
  for (int i = 0; i < fileNames.size(); ++i)
  {
    std::string fileName = fileNames[i];
    std::cout << "Testing: " << fileName << std::endl;
    if (!reader->CanReadFile(fileName.c_str()))
    {
      return EXIT_FAILURE;
    }
    reader->SetFileName(fileName.c_str());
    reader->Update();
  }

  return EXIT_SUCCESS;
}
