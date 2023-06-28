/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUTextureView.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUTextureView.h"

#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtkWebGPUSampler.h"
#include "vtkWebGPUTexture.h"
#include "vtkWebGPUType.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUTextureView);
vtkCxxSetObjectMacro(vtkWebGPUTextureView, Texture, vtkWebGPUTexture);
vtkCxxSetObjectMacro(vtkWebGPUTextureView, Sampler, vtkWebGPUSampler);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUTextureView::vtkInternal
{
public:
  WGPUTextureView TextureView;
  WGPUBindGroupLayoutEntry BindGroupLayoutEntry = {};
  WGPUTextureBindingLayout TextureBindingLayout = {};
  WGPUBindGroupEntry BindGroupEntry = {};
  vtkInternal();
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUTextureView::vtkInternal::vtkInternal()
{
  this->TextureBindingLayout.nextInChain = nullptr;
  this->TextureBindingLayout.sampleType = WGPUTextureSampleType_Float;
  this->TextureBindingLayout.viewDimension = WGPUTextureViewDimension_2D;

  this->BindGroupLayoutEntry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
  this->BindGroupLayoutEntry.texture = this->TextureBindingLayout;

  this->BindGroupEntry.nextInChain = nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUTextureView::vtkWebGPUTextureView()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUTextureView");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUTextureView::~vtkWebGPUTextureView()
{
  this->Destroy();
  this->SetTexture(nullptr);
  this->SetSampler(nullptr);
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUTextureView::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Format = " << this->Format << endl;
  os << indent << "Dimension = " << this->Dimension << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUTextureView::Create()
{
  if (!this->Texture)
  {
    vtkErrorMacro(<< "Cannot create texture view without texture");
    return;
  }

  WGPUTexture textureHandle = reinterpret_cast<WGPUTexture>(this->Texture->GetHandle());
  if (!textureHandle)
  {
    this->Texture->Create(vtkWebGPUInstance::GetInstance()->GetDevice());
    textureHandle = reinterpret_cast<WGPUTexture>(this->Texture->GetHandle());
    if (!textureHandle)
    {
      vtkErrorMacro(<< "Texture for view could not be created");
      return;
    }
  }

  WGPUTextureViewDescriptor desc = {};
  desc.nextInChain = nullptr;
  desc.label = this->GetLabel();
  switch (this->Dimension)
  {
    case TEXTUREVIEW_3D:
      desc.dimension = WGPUTextureViewDimension_3D;
      break;
    default:
    case TEXTUREVIEW_2D:
      desc.dimension = WGPUTextureViewDimension_2D;
      break;
  }

  // Update the bindgroup layout entry
  this->Internal->TextureBindingLayout.viewDimension = desc.dimension;
  bool ok = true;
  this->Internal->TextureBindingLayout.sampleType =
    vtkWebGPUType::GetTextureSampleTypeFromFormat(this->Texture->GetFormat(), ok);
  this->Internal->BindGroupLayoutEntry.texture = this->Internal->TextureBindingLayout;

  // All set, create the view
  this->Internal->TextureView = wgpuTextureCreateView(textureHandle, &desc);
  this->BindGroupTime.Modified();
  this->Modified();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUTextureView::Destroy()
{
  if (!this->Internal->TextureView)
  {
    return;
  }
  wgpuTextureViewRelease(this->Internal->TextureView);
  this->Internal->TextureView = nullptr;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUTextureView::GetHandle()
{
  return reinterpret_cast<void*>(this->Internal->TextureView);
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUTextureView::GetBindGroupEntry()
{
  if (!this->Texture)
  {
    vtkErrorMacro(<< "No bind group entry without texture");
    return nullptr;
  }

  if (!this->GetHandle())
  {
    this->Create();
  }
  // reset the texture handle in case it has changed.
  this->Internal->BindGroupEntry.textureView = this->Internal->TextureView;
  if (this->Sampler)
  {
    if (!this->Sampler->GetHandle())
    {
      this->Sampler->Create();
    }
    this->Internal->BindGroupEntry.sampler =
      reinterpret_cast<WGPUSampler>(this->Sampler->GetHandle());
  }
  return reinterpret_cast<void*>(&this->Internal->BindGroupEntry);
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUTextureView::GetBindGroupLayoutEntry()
{
  return reinterpret_cast<void*>(&this->Internal->BindGroupLayoutEntry);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
