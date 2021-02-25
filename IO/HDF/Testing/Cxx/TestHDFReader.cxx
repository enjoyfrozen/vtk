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

#include "vtkHDFReader.h"
#include "vtkImageData.h"
#include "vtkNew.h"
#include "vtkTesting.h"
#include <string>

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

  return EXIT_SUCCESS;
}
