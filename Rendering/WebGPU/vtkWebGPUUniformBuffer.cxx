/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUUniformBuffer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUUniformBuffer.h"
#include "vtkObjectFactory.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUUniformBuffer);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUUniformBuffer::vtkInternal
{
public:
  WGPUBufferBindingLayout BufferBindingLayout = {};
  vtkInternal();
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUUniformBuffer::vtkInternal::vtkInternal()
{
  this->BufferBindingLayout.nextInChain = nullptr;
  this->BufferBindingLayout.type = WGPUBufferBindingType_Uniform;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUUniformBuffer::vtkWebGPUUniformBuffer()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUUniformBuffer");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUUniformBuffer::~vtkWebGPUUniformBuffer()
{
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUUniformBuffer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUUniformBuffer::GetBindGroupLayoutEntry()
{
  WGPUBindGroupLayoutEntry* e =
    reinterpret_cast<WGPUBindGroupLayoutEntry*>(this->Superclass::GetBindGroupLayoutEntry());
  e->buffer = this->Internal->BufferBindingLayout;
  return reinterpret_cast<void*>(e);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
