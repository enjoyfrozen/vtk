/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkUnsignedLongLongArray.cxx

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
VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATE(unsigned long long);

VTK_ABI_NAMESPACE_END

#include "vtkUnsignedLongLongArray.h"

#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkUnsignedLongLongArray);
vtkStandardExtendedNewMacro(vtkUnsignedLongLongArray);

//------------------------------------------------------------------------------
vtkUnsignedLongLongArray::vtkUnsignedLongLongArray() = default;

//------------------------------------------------------------------------------
vtkUnsignedLongLongArray::~vtkUnsignedLongLongArray() = default;

//------------------------------------------------------------------------------
void vtkUnsignedLongLongArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->RealSuperclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END
