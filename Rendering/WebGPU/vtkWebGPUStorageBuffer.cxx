/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUStorageBuffer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUStorageBuffer.h"
#include "vtkObjectFactory.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUStorageBuffer);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUStorageBuffer::vtkInternal
{
public:
  WGPUBufferBindingLayout BufferBindingLayout = {};
  vtkInternal();
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUStorageBuffer::vtkInternal::vtkInternal()
{
  this->BufferBindingLayout.nextInChain = nullptr;
  this->BufferBindingLayout.type = WGPUBufferBindingType_Storage;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUStorageBuffer::vtkWebGPUStorageBuffer()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUStorageBuffer");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUStorageBuffer::~vtkWebGPUStorageBuffer()
{
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUStorageBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUStorageBuffer::GetBindGroupLayoutEntry()
{
  WGPUBindGroupLayoutEntry* e =
    reinterpret_cast<WGPUBindGroupLayoutEntry*>(this->Superclass::GetBindGroupLayoutEntry());
  e->buffer = this->Internal->BufferBindingLayout;
  return reinterpret_cast<void*>(e);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
