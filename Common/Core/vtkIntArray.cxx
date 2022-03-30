/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkIntArray.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Instantiate superclass first to give the template a DLL interface.
#define VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATING
#include "vtkAOSDataArrayTemplate.txx"
VTK_ABI_NAMESPACE_BEGIN
VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATE(int);

VTK_ABI_NAMESPACE_END
#include "vtkIntArray.h"

#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkIntArray);
vtkStandardExtendedNewMacro(vtkIntArray);

//------------------------------------------------------------------------------
vtkIntArray::vtkIntArray() = default;

//------------------------------------------------------------------------------
vtkIntArray::~vtkIntArray() = default;

//------------------------------------------------------------------------------
void vtkIntArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->RealSuperclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END
