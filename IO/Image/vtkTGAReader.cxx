/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTGAReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkTGAReader.h"

#include "vtkObjectFactory.h"

#include <fstream>

vtkStandardNewMacro(vtkTGAReader);

//----------------------------------------------------------------------------
void vtkTGAReader::ExecuteInformation()
{
  this->ComputeInternalFileName(0);
  std::ifstream file(this->InternalFileName, std::ios::binary);

  char header[18];
  file.read(header, 18 * sizeof(char));

  file.close();

  this->FileLowerLeft = 1;

  this->DataOrigin[0] = static_cast<double>(*reinterpret_cast<short*>(&header[8]));
  this->DataOrigin[1] = static_cast<double>(*reinterpret_cast<short*>(&header[10]));
  this->DataOrigin[2] = 0.0;

  this->DataExtent[0] = 0;
  this->DataExtent[1] = static_cast<int>(*reinterpret_cast<short*>(&header[12]) - 1);
  this->DataExtent[2] = 0;
  this->DataExtent[3] = static_cast<int>(*reinterpret_cast<short*>(&header[14]) - 1);

  this->SetHeaderSize(18);
  this->SetDataScalarTypeToUnsignedChar();
  this->SetNumberOfScalarComponents(4);

  this->vtkImageReader::ExecuteInformation();
}

//----------------------------------------------------------------------------
int vtkTGAReader::CanReadFile(const char* fname)
{
  std::ifstream file(fname, std::ios::binary);

  if (!file.is_open())
  {
    return 0;
  }

  char header[18];
  file.read(header, 18 * sizeof(char));

  // only 32 bits uncompressed colors
  if (header[2] != 2 || header[16] != 32)
  {
    vtkWarningMacro("This TGA file is not supported");
    return 0;
  }

  return 1;
}
