/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUCommandEncoder.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUCommandEncoder.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
class vtkWebGPUCommandEncoder::vtkInternal
{
public:
  vtkInternal();
  WGPUCommandEncoder Encoder = nullptr;
  WGPUCommandEncoderDescriptor Descriptor = {};
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUCommandEncoder::vtkInternal::vtkInternal()
{
  this->Descriptor.nextInChain = nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUCommandEncoder);

//-------------------------------------------------------------------------------------------------
vtkWebGPUCommandEncoder::vtkWebGPUCommandEncoder()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUCommandEncoder");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUCommandEncoder::~vtkWebGPUCommandEncoder()
{
  this->Destroy();
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUCommandEncoder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUCommandEncoder::GetHandle()
{
  return reinterpret_cast<void*>(this->Internal->Encoder);
}

//-------------------------------------------------------------------------------------------------
WGPUCommandEncoderDescriptor& vtkWebGPUCommandEncoder::GetDescriptor()
{
  return this->Internal->Descriptor;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUCommandEncoder::Create()
{
  auto inst = vtkWebGPUInstance::GetInstance();
  if (!inst->IsValid())
  {
    vtkErrorMacro(<< "Cannot create command encoder without a valid webgpu instance.");
    return;
  }
  this->Internal->Descriptor.label = this->Label;
  this->Internal->Encoder =
    wgpuDeviceCreateCommandEncoder(inst->GetDevice()->GetHandle(), &this->Internal->Descriptor);
  if (this->Label)
  {
    wgpuCommandEncoderPushDebugGroup(this->Internal->Encoder, this->Label);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUCommandEncoder::Destroy()
{
  if (!this->Internal->Encoder)
  {
    return;
  }
  if (this->Label)
  {
    wgpuCommandEncoderPopDebugGroup(this->Internal->Encoder);
  }
  wgpuCommandEncoderRelease(this->Internal->Encoder);
  this->Internal->Encoder = nullptr;
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
