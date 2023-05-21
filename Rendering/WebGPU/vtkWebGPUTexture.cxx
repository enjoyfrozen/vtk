/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUTexture.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPUTexture.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUTexture);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUTexture::vtkInternal
{
public:
  WGPUTexture Texture;
};

//-------------------------------------------------------------------------------------------------
vtkWebGPUTexture::vtkWebGPUTexture()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPUTexture");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUTexture::~vtkWebGPUTexture()
{
  this->Destroy();
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUTexture::Create(vtkWebGPUDevice* d)
{
  if (!d)
  {
    vtkErrorMacro(<< "Cannot create a texture without a device");
    return;
  }

  WGPUExtent3D texExtent;
  texExtent.width = this->Dimensions[0];
  texExtent.height = this->Dimensions[1];
  texExtent.depthOrArrayLayers = this->Dimensions[2];

  WGPUTextureDescriptor desc;
  desc.nextInChain = nullptr;
  desc.size = texExtent;
  if (this->Format <= WGPUTextureFormat_Undefined || this->Format >= WGPUTextureFormat_Force32)
  {
    // Default to RGBA8 UNorm
    desc.format = WGPUTextureFormat_RGBA8Unorm;
  }
  desc.label = this->GetLabel();
  desc.mipLevelCount = this->MIPLevel;
  desc.dimension = this->Dimensions[2] == 1 ? WGPUTextureDimension_2D : WGPUTextureDimension_3D;
  desc.sampleCount = 1;
  desc.usage = this->UsageFlags;
  desc.viewFormatCount = 0;
  desc.viewFormats = nullptr;

  this->Internal->Texture = wgpuDeviceCreateTexture(d->GetHandle(), &desc);
  this->Modified();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUTexture::Destroy()
{
  if (!this->Internal->Texture)
  {
    return;
  }
  wgpuTextureRelease(this->Internal->Texture);
  this->Internal->Texture = nullptr;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUTexture::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Dimensions = [" << this->Dimensions[0] << ", " << this->Dimensions[1] << ", "
     << this->Dimensions[2] << "]" << endl;
  os << indent << "MIPLevel = " << this->MIPLevel << endl;
  os << indent << "Format = " << this->Format << endl;
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPUTexture::GetHandle()
{
  return reinterpret_cast<void*>(this->Internal->Texture);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
