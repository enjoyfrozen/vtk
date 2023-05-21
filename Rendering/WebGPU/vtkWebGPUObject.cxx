/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUObject.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUObject.h"

#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkWebGPUObject::vtkWebGPUObject() = default;

//-------------------------------------------------------------------------------------------------
vtkWebGPUObject::~vtkWebGPUObject()
{
  delete[] this->Label;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUObject::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Label = " << (this->Label ? this->Label : "(null)") << endl;
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
