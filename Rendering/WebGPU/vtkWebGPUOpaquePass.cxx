/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUOpaquePass.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUOpaquePass.h"

#include "vtkObjectFactory.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtkWebGPURenderPassEncoder.h"
#include "vtkWebGPURendererNode.h"
#include "vtkWebGPUTexture.h"
#include "vtkWebGPUTextureView.h"
#include "vtkWebGPUViewNode.h"
#include "vtkWebGPUWindowNode.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUOpaquePass);

//-------------------------------------------------------------------------------------------------
vtkWebGPUOpaquePass::vtkWebGPUOpaquePass() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUOpaquePass::~vtkWebGPUOpaquePass()
{
  if (this->RenderEncoder)
  {
    this->RenderEncoder->Delete();
    this->RenderEncoder = nullptr;
  }
  if (this->ColorTexture)
  {
    this->ColorTexture->Delete();
    this->ColorTexture = nullptr;
  }
  if (this->ColorTextureView)
  {
    this->ColorTextureView->Delete();
    this->ColorTextureView = nullptr;
  }
  if (this->DepthTexture)
  {
    this->DepthTexture->Delete();
    this->DepthTexture = nullptr;
  }
  if (this->DepthTextureView)
  {
    this->DepthTextureView->Delete();
    this->DepthTextureView = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUOpaquePass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUOpaquePass::Traverse(vtkViewNode* vn, vtkRenderPass* parent)
{
  vtkWebGPURendererNode* renNode = vtkWebGPURendererNode::SafeDownCast(vn);
  if (!renNode)
  {
    vtkErrorMacro("Opaque pass expects the view node to be a vtkWebGPURendererNode");
    return;
  }

  this->CurrentParent = parent;
  vtkWebGPUInstance* inst = vtkWebGPUInstance::GetInstance();
  if (!inst->IsValid())
  {
    // If the instance is not valid, abort
    return;
  }

  vtkWebGPUDevice* device = inst->GetDevice();

  vtkWebGPUWindowNode* wn = vtkWebGPUWindowNode::SafeDownCast(renNode->GetParent());
  int* size = wn->GetSize();

  if (!this->RenderEncoder)
  {
    this->RenderEncoder = vtkWebGPURenderPassEncoder::New();
    this->RenderEncoder->SetLabel("OpaquePass");
    this->RenderEncoder->SetColorLoadOp(WGPULoadOp_Load);

    this->RenderEncoder->ClearColorTextureViews();

    if (this->ColorTexture)
    {
      this->ColorTexture->Delete();
      this->ColorTexture = nullptr;
    }
    this->ColorTexture = vtkWebGPUTexture::New();
    this->ColorTexture->SetLabel("OpaquePassColorTexture");
    this->ColorTexture->SetDimensions(size[0], size[1], 1);
    this->ColorTexture->SetFormat(WGPUTextureFormat_RGBA16Float);
    this->ColorTexture->SetUsageFlags(WGPUTextureUsage_RenderAttachment |
      WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopySrc);
    this->ColorTexture->Create(device);

    if (this->ColorTextureView)
    {
      this->ColorTextureView->Delete();
      this->ColorTextureView = nullptr;
    }
    this->ColorTextureView = vtkWebGPUTextureView::New();
    this->ColorTextureView->SetTexture(this->ColorTexture);
    this->ColorTextureView->SetLabel("OpaquePassColorTextureView");
    this->ColorTextureView->Create();
    this->RenderEncoder->AddColorTextureView(this->ColorTextureView);

    if (this->DepthTexture)
    {
      this->DepthTexture->Delete();
      this->DepthTexture = nullptr;
    }
    this->DepthTexture = vtkWebGPUTexture::New();
    this->DepthTexture->SetLabel("OpaquePassDepthTexture");
    this->DepthTexture->SetDimensions(size[0], size[1], 1);
    this->DepthTexture->SetFormat(WGPUTextureFormat_Depth32Float);
    this->DepthTexture->SetUsageFlags(WGPUTextureUsage_RenderAttachment |
      WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopySrc);
    if (this->DepthTextureView)
    {
      this->DepthTextureView->Delete();
      this->DepthTextureView = nullptr;
    }
    this->DepthTextureView = vtkWebGPUTextureView::New();
    this->DepthTextureView->SetTexture(this->DepthTexture);
    this->DepthTextureView->SetLabel("OpaquePassDepthTextureView");
    this->DepthTextureView->Create();
    this->RenderEncoder->SetDepthTextureView(this->DepthTextureView);
  }
  else
  {
    if (this->ColorTexture)
    {
      // Resize
      auto mtime = this->ColorTexture->GetMTime();
      this->ColorTexture->SetDimensions(size[0], size[1], 1);
      if (this->ColorTexture->GetMTime() > mtime)
      {
        // Only re-create if the size changed
        this->ColorTexture->Destroy();
        this->ColorTexture->Create(device);
      }
    }
    if (this->DepthTexture)
    {
      // Resize
      auto mtime = this->DepthTexture->GetMTime();
      this->DepthTexture->SetDimensions(size[0], size[1], 1);
      if (this->DepthTexture->GetMTime() > mtime)
      {
        // Only re-create if the size changed
        this->DepthTexture->Destroy();
        this->DepthTexture->Create(device);
      }
    }
  }

  this->RenderEncoder->AttachTextureViews();
  renNode->SetRenderEncoder(this->RenderEncoder);
  renNode->Traverse(vtkWebGPUViewNode::opaquePass);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
