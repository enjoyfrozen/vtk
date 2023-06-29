/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURenderPassEncoder.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPURenderPassEncoder.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUCommandEncoder.h"
#include "vtkWebGPUInstance.h"
#include "vtkWebGPUMapperNode.h"
#include "vtkWebGPURenderPipeline.h"
#include "vtkWebGPUTextureView.h"
#include "vtk_wgpu.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
class vtkWebGPURenderPassEncoder::vtkInternal
{
public:
  vtkInternal();
  WGPURenderPassEncoder Encoder = nullptr;
  WGPURenderPassDescriptor Descriptor = {};
  std::vector<WGPURenderPassColorAttachment> ColorAttachments;
  WGPURenderPassDepthStencilAttachment DepthAttachment;
};

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPassEncoder::vtkInternal::vtkInternal()
{
  this->Descriptor.nextInChain = nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPURenderPassEncoder);
vtkCxxSetObjectMacro(vtkWebGPURenderPassEncoder, DepthTextureView, vtkWebGPUTextureView);

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPassEncoder::vtkWebGPURenderPassEncoder()
{
  this->Internal = new vtkInternal();
  this->SetLabel("VTKWebGPURenderEncoder");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderPassEncoder::~vtkWebGPURenderPassEncoder()
{
  this->ClearColorTextureViews();
  this->SetDepthTextureView(nullptr);
  this->End();
  if (this->Internal)
  {
    delete this->Internal;
    this->Internal = nullptr;
  }
}

//-------------------------------------------------------------------------------------------------
void* vtkWebGPURenderPassEncoder::GetHandle()
{
  return reinterpret_cast<void*>(this->Internal->Encoder);
}

//-------------------------------------------------------------------------------------------------
WGPURenderPassDescriptor& vtkWebGPURenderPassEncoder::GetDescriptor()
{
  return this->Internal->Descriptor;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::Begin()
{
  vtkWebGPUInstance* inst = vtkWebGPUInstance::GetInstance();
  if (!inst->IsValid())
  {
    return;
  }
  // Clear the drawing pipelines
  this->DrawingMappers.clear();

  this->Internal->Descriptor.nextInChain = nullptr;
  this->Internal->Descriptor.timestampWriteCount = 0;
  this->Internal->Descriptor.timestampWrites = nullptr;
  this->Internal->Descriptor.label = this->Label;
  this->Internal->Encoder = wgpuCommandEncoderBeginRenderPass(
    static_cast<WGPUCommandEncoder>(inst->GetCommandEncoder()->GetHandle()),
    &this->Internal->Descriptor);
  if (this->Label)
  {
    wgpuRenderPassEncoderPushDebugGroup(this->Internal->Encoder, this->Label);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::Draw()
{
  if (!this->Internal->Encoder)
  {
    vtkErrorMacro(<< "No encoder present. Call Begin() before Draw()");
    return;
  }

  // Iterate over the drawing mappers and draw
  for (const auto& it : this->DrawingMappers)
  {
    vtkWebGPURenderPipeline* rp = vtkWebGPURenderPipeline::SafeDownCast(it.first);
    if (!rp)
    {
      continue;
    }
    // Bind the pipeline
    this->SetPipeline(rp);
    // Now iterate over all the mappers
    for (vtkWebGPUMapperNode* mapper : it.second)
    {
      // Ensure that the render pass encoder is updated on the mapper
      mapper->SetRenderPassEncoder(this);
      // Draw
      mapper->Draw();
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::End()
{
  if (!this->Internal->Encoder)
  {
    return;
  }
  if (this->Label)
  {
    wgpuRenderPassEncoderPopDebugGroup(this->Internal->Encoder);
  }
  wgpuRenderPassEncoderEnd(this->Internal->Encoder);
  wgpuRenderPassEncoderRelease(this->Internal->Encoder);
  this->Internal->Encoder = nullptr;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::AddColorTextureView(vtkWebGPUTextureView* v)
{
  if (!v)
  {
    return;
  }
  v->Register(this);
  this->ColorTextureViews.push_back(v);
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUTextureView* vtkWebGPURenderPassEncoder::GetColorTextureView(int idx)
{
  if (idx >= static_cast<int>(this->ColorTextureViews.size()))
  {
    return nullptr;
  }
  return this->ColorTextureViews.at(idx);
}

//-------------------------------------------------------------------------------------------------
std::size_t vtkWebGPURenderPassEncoder::GetNumberOfColorTextureViews()
{
  return this->ColorTextureViews.size();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::ClearColorTextureViews()
{
  for (vtkWebGPUTextureView* v : this->ColorTextureViews)
  {
    if (v)
    {
      v->UnRegister(this);
    }
  }
  this->ColorTextureViews.clear();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::AttachTextureViews()
{
  std::size_t numColorViews = this->GetNumberOfColorTextureViews();
  this->Internal->Descriptor.colorAttachmentCount = static_cast<uint32_t>(numColorViews);
  this->Internal->ColorAttachments.resize(numColorViews);
  for (std::size_t i = 0; i < numColorViews; ++i)
  {
    vtkWebGPUTextureView* v = this->ColorTextureViews[i];
    WGPURenderPassColorAttachment a = {};
    a.view = static_cast<WGPUTextureView>(v->GetHandle());
    a.loadOp = static_cast<WGPULoadOp>(this->ColorLoadOp);
    a.storeOp = static_cast<WGPUStoreOp>(this->ColorStoreOp);
    a.clearValue = WGPUColor{ this->ClearColor[0], this->ClearColor[1], this->ClearColor[2],
      this->ClearColor[3] };
    a.resolveTarget = nullptr;
    this->Internal->ColorAttachments[i] = a;
  }
  this->Internal->Descriptor.colorAttachments = this->Internal->ColorAttachments.data();

  this->Internal->DepthAttachment = {};
  this->Internal->DepthAttachment.view =
    static_cast<WGPUTextureView>(this->DepthTextureView->GetHandle());
  this->Internal->DepthAttachment.depthLoadOp = static_cast<WGPULoadOp>(this->DepthLoadOp);
  this->Internal->DepthAttachment.depthStoreOp = static_cast<WGPUStoreOp>(this->DepthStoreOp);
  this->Internal->DepthAttachment.depthClearValue = static_cast<float>(this->ClearDepth);
  this->Internal->DepthAttachment.stencilLoadOp = static_cast<WGPULoadOp>(this->StencilLoadOp);
  this->Internal->DepthAttachment.stencilStoreOp = static_cast<WGPUStoreOp>(this->StencilStoreOp);
  this->Internal->DepthAttachment.stencilClearValue = this->ClearStencil;
  this->Internal->Descriptor.depthStencilAttachment = &this->Internal->DepthAttachment;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::SetPipeline(vtkWebGPUPipeline* pl)
{
  if (this->Pipeline == pl)
  {
    return;
  }

  vtkWebGPURenderPipeline* pipeline = vtkWebGPURenderPipeline::SafeDownCast(pl);

  if (!this->Internal->Encoder)
  {
    vtkErrorMacro(<< "Set pipeline on the encoder after calling Begin.");
    return;
  }

  WGPURenderPipeline* plHandle = reinterpret_cast<WGPURenderPipeline*>(pipeline->GetHandle());
  if (!plHandle)
  {
    vtkErrorMacro(<< "Could not get pipeline handle.");
    return;
  }
  wgpuRenderPassEncoderSetPipeline(this->Internal->Encoder, *plHandle);

  // Check attachments state
  WGPURenderPipelineDescriptor* desc = pipeline->GetDescriptor();
  if (this->GetNumberOfColorTextureViews() != desc->fragment->targetCount)
  {
    vtkWarningMacro(<< "Color attachment counts on pipeline = "
                    << this->GetNumberOfColorTextureViews() << " while encoder has "
                    << desc->fragment->targetCount);
  }

  // Check depth buffer
  if (!this->Internal->DepthAttachment.view != !desc->depthStencil)
  {
    vtkWarningMacro(<< "Mismatched depth texture attachments between pipeline and encoder.");
  }

  // Forward to superclass
  this->Superclass::SetPipeline(pl);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderPassEncoder::RegisterPipelineMapper(
  vtkWebGPUPipeline* pl, vtkWebGPUMapperNode* mapper)
{
  if (this->DrawingMappers.find(pl) != this->DrawingMappers.end())
  {
    // If the pipeline is already added, just add the new mapper
    this->DrawingMappers[pl].push_back(mapper);
    return;
  }
  std::vector<vtkWebGPUMapperNode*> mappers(1);
  mappers[0] = mapper;
  this->DrawingMappers[pl] = mappers;
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
