/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUBindableObject.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*=============================================================================
Copyright and License information
=============================================================================*/

// vtk includes
#include "vtkWebGPUBindableObject.h"
#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkWebGPUBindableObject::vtkWebGPUBindableObject() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUBindableObject::~vtkWebGPUBindableObject() {}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUBindableObject::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "BindGroupTime = " << this->GetBindGroupTime() << endl;
}

//-------------------------------------------------------------------------------------------------
vtkMTimeType vtkWebGPUBindableObject::GetBindGroupTime() const
{
  return this->BindGroupTime.GetMTime();
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
