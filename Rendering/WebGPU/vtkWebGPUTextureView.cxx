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
/*=============================================================================
Copyright and License information
=============================================================================*/

// vtk includes
#include "vtkWebGPUTextureView.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtkWebGPUTexture.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUTextureView);
vtkCxxSetObjectMacro(vtkWebGPUTextureView, Texture, vtkWebGPUTexture);

//-------------------------------------------------------------------------------------------------
class vtkWebGPUTextureView::vtkInternal
{
public:
  WGPUTextureView TextureView;
};

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

  // All set, create the view
  this->Internal->TextureView = wgpuTextureCreateView(textureHandle, &desc);
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
VTK_ABI_NAMESPACE_END
