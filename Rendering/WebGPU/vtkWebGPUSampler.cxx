/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUSampler.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUSampler.h"

#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUSampler);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUSampler::vtkInternal
{
public:
  WGPUSampler Sampler = nullptr;
  WGPUBindGroupLayoutEntry BindGroupLayoutEntry = {};
  WGPUSamplerBindingLayout SamplerBindingLayout = {};
  WGPUBindGroupEntry BindGroupEntry = {};
  WGPUSamplerDescriptor Descriptor = {};
  vtkInternal();
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUSampler::vtkInternal::vtkInternal()
{
  this->Descriptor.nextInChain = nullptr;

  this->SamplerBindingLayout.nextInChain = nullptr;
  this->SamplerBindingLayout.type = WGPUSamplerBindingType_Filtering;

  this->BindGroupLayoutEntry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
  this->BindGroupLayoutEntry.sampler = this->SamplerBindingLayout;

  this->BindGroupEntry.nextInChain = nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUSampler::vtkWebGPUSampler()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUSampler");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUSampler::~vtkWebGPUSampler()
{
  this->Destroy();
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUSampler::Create()
{
  vtkWebGPUInstance* inst = vtkWebGPUInstance::GetInstance();
  if (!inst)
  {
    vtkErrorMacro(<< "Cannot create a sampler without a valid webgpu instance");
    return;
  }
  this->Internal->Descriptor.label = this->GetLabel();
  this->Internal->Descriptor.addressModeU = static_cast<WGPUAddressMode>(this->AddressModeU);
  this->Internal->Descriptor.addressModeV = static_cast<WGPUAddressMode>(this->AddressModeV);
  this->Internal->Descriptor.addressModeW = static_cast<WGPUAddressMode>(this->AddressModeW);
  this->Internal->Descriptor.minFilter = static_cast<WGPUFilterMode>(this->MinFilter);
  this->Internal->Descriptor.magFilter = static_cast<WGPUFilterMode>(this->MagFilter);
  this->Internal->Descriptor.mipmapFilter = static_cast<WGPUMipmapFilterMode>(this->MipMapFilter);
  this->Internal->Sampler =
    wgpuDeviceCreateSampler(inst->GetDevice()->GetHandle(), &this->Internal->Descriptor);
  this->BindGroupTime.Modified();
  this->Modified();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUSampler::Destroy()
{
  if (!this->Internal->Sampler)
  {
    return;
  }
  wgpuSamplerRelease(this->Internal->Sampler);
  this->Internal->Sampler = nullptr;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUSampler::GetHandle()
{
  return reinterpret_cast<void*>(this->Internal->Sampler);
}

//-------------------------------------------------------------------------------------------------
WGPUSamplerDescriptor& vtkWebGPUSampler::GetDescriptor()
{
  return this->Internal->Descriptor;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUSampler::GetBindGroupEntry()
{
  if (!this->GetHandle())
  {
    this->Create();
  }
  // reset the sampler handle in case it has changed.
  this->Internal->BindGroupEntry.sampler = this->Internal->Sampler;
  return reinterpret_cast<void*>(&this->Internal->BindGroupEntry);
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUSampler::GetBindGroupLayoutEntry()
{
  return reinterpret_cast<void*>(&this->Internal->BindGroupLayoutEntry);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUSampler::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << " AddressModeU = " << this->AddressModeU << endl;
  os << indent << " AddressModeV = " << this->AddressModeV << endl;
  os << indent << " AddressModeW = " << this->AddressModeW << endl;
  os << indent << " MinFilter = " << this->MinFilter << endl;
  os << indent << " MagFilter = " << this->MagFilter << endl;
  os << indent << " MipMapFilter = " << this->MipMapFilter << endl;
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
