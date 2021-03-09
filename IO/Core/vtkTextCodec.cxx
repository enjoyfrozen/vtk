/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkSQLDatabase.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/

#include "vtkTextCodec.h"
#include <vtk_utf8.h>

const char* vtkTextCodec::Name()
{
  return "";
}

bool vtkTextCodec::CanHandle(const char*)
{
  return false;
}

bool vtkTextCodec::IsValid(istream&)
{
  return false;
}

vtkTextCodec::~vtkTextCodec() = default;

vtkTextCodec::vtkTextCodec() = default;

namespace
{
class vtkStringOutputIterator : public vtkTextCodec::OutputIterator
{
public:
  vtkStringOutputIterator& operator++(int) override { return *this; }
  vtkStringOutputIterator& operator*() override { return *this; }
  vtkStringOutputIterator& operator=(const vtkTypeUInt32 value) override
  {
    utf8::append(value, std::back_inserter(this->Output));
    return *this;
  }

  vtkStringOutputIterator(std::string& output)
    : Output(output)
  {
  }

private:
  std::string& Output;
};
}

std::string vtkTextCodec::ToString(istream& inputStream)
{
  std::string result;

  vtkStringOutputIterator iterator(result);
  this->ToUnicode(inputStream, iterator);

  return result;
}

void vtkTextCodec::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "vtkTextCodec (" << this << ") \n";
  indent = indent.GetNextIndent();
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
}
