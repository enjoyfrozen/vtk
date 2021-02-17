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
  std::string fileName = dataRoot + "/Data/wavelet-vti.hdf";
  vtkNew<vtkHDFReader> reader;
  if (!reader->CanReadFile(fileName.c_str()))
  {
    return EXIT_FAILURE;
  }
  reader->SetFileName(fileName.c_str());
  reader->Update();
  return EXIT_SUCCESS;
}
