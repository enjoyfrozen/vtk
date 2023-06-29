/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUMapperNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUMapperNode.h"
#include "vtkObjectFactory.h"
#include "vtkWebGPUBindGroup.h"
#include "vtkWebGPUDevice.h"
#include "vtkWebGPUInstance.h"
#include "vtkWebGPURenderPassEncoder.h"
#include "vtkWebGPURenderPipeline.h"
#include "vtkWebGPURendererNode.h"
#include "vtkWebGPUSampler.h"
#include "vtkWebGPUStorageBuffer.h"
#include "vtkWebGPUTextureView.h"
#include "vtkWebGPUUniformBuffer.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUMapperNode);
vtkCxxSetObjectMacro(vtkWebGPUMapperNode, Pipeline, vtkWebGPUPipeline);
vtkCxxSetObjectMacro(vtkWebGPUMapperNode, RenderPassEncoder, vtkWebGPURenderPassEncoder);
vtkCxxSetObjectMacro(vtkWebGPUMapperNode, Renderer, vtkWebGPURendererNode);
vtkCxxSetObjectMacro(vtkWebGPUMapperNode, SSBO, vtkWebGPUStorageBuffer);
vtkCxxSetObjectMacro(vtkWebGPUMapperNode, UBO, vtkWebGPUUniformBuffer);

//-------------------------------------------------------------------------------------------------
vtkWebGPUMapperNode::vtkWebGPUMapperNode()
{
  this->RenderPassEncoder = nullptr;
  this->BindGroup = vtkWebGPUBindGroup::New();
  this->BindGroup->SetLabel("MapperBindGroup");
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUMapperNode::~vtkWebGPUMapperNode()
{
  if (this->RenderPassEncoder)
  {
    this->RenderPassEncoder->Delete();
    this->RenderPassEncoder = nullptr;
  }
  if (this->BindGroup)
  {
    this->BindGroup->Delete();
    this->BindGroup = nullptr;
  }
  if (this->Pipeline)
  {
    this->Pipeline->Delete();
    this->Pipeline = nullptr;
  }
  this->Bindables.clear();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << " RenderPassEncoder: ";
  if (this->RenderPassEncoder)
  {
    this->RenderPassEncoder->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << "(null)" << endl;
  }
  os << indent << " Pipeline: ";
  if (this->Pipeline)
  {
    this->Pipeline->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << "(null)" << endl;
  }
  os << indent << " BindGroup: ";
  if (this->BindGroup)
  {
    this->BindGroup->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << "(null)" << endl;
  }
  os << indent << " UBO: ";
  if (this->UBO)
  {
    this->UBO->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << "(null)" << endl;
  }
  os << indent << " SSBO: ";
  if (this->SSBO)
  {
    this->SSBO->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << "(null)" << endl;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::PrepareAndDraw()
{
  if (!this->RenderPassEncoder || !this->Pipeline)
  {
    vtkWarningMacro(<< "Mapper's draw called without a render pass encoder.");
    return;
  }
  this->PrepareToDraw();
  this->RenderPassEncoder->SetPipeline(this->GetPipeline());
  this->Draw();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::PrepareToDraw()
{
  if (!this->RenderPassEncoder)
  {
    return;
  }

  // Do anything needed to get our input data up to date
  this->UpdateInput();

  // Make sure buffers are created and up to date
  this->UpdateBuffers();

  // Update bindings and bind groups/layouts
  // Does not actually bind them. That is done in Draw
  this->UpdateBindings();

  // Update the pipeline, shader code, etc.
  this->UpdatePipeline();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::Draw()
{
  // At this time, the pipeline should be bound by vtkWebGPURenderPassEncoder::SetPipeline
  vtkWebGPURenderPipeline* pl =
    vtkWebGPURenderPipeline::SafeDownCast(this->RenderPassEncoder->GetPipeline());
  if (!pl)
  {
    return;
  }

  // Bind our bind group to the render pass encoder
  this->RenderPassEncoder->ActivateBindGroup(this->BindGroup);

  // Activate the renderer's bind group
  if (this->Renderer)
  {
    this->Renderer->SetRenderEncoder(this->RenderPassEncoder);
    this->Renderer->ActivateBindGroup();
  }

  // Bind the vertex input
  // pl->BindVertexInput(this->VertexInput);

  // auto indexBuffer = this->VertexInput->GetIndexBuffer();
  // Actual draw call
  // if (indexBuffer)
  // {
  //   this->RenderPassEncoder->DrawIndexed(indexBuffer->GetIndexCount(), this->NumberOfInstances,0,
  //   0, 0);
  // }
  // else
  // {
  //   this->RenderPassEncoder->DrawIndexed(this->NumberOfVertices, this->NumberOfInstances,0, 0);
  // }

}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdateInput()
{
  // Nothing to update
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdateBuffers()
{
  // Nothing to update
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdateBindings()
{
  // bindings can change without a pipeline change
  // as long as their layout remains the same.
  // That is why this is done even when the pipeline
  // doesn't change.
  this->BindGroup->SetBindables(this->GetBindables());
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdatePipeline()
{
  vtkWebGPUDevice* device = vtkWebGPUInstance::GetInstance()->GetDevice();
  this->ComputePipelineHash();
  this->Pipeline = device->GetPipeline(this->PipelineHash);

  if (!this->Pipeline)
  {
    this->Pipeline = vtkWebGPURenderPipeline::New();
    vtkWebGPURenderPipeline* p = static_cast<vtkWebGPURenderPipeline*>(this->Pipeline);

    if (this->Renderer)
    {
      p->AddBindGroup(this->Renderer->GetBindGroup());
    }

    p->AddBindGroup(this->BindGroup);

    this->GenerateShaderDescriptions(p);
    p->SetTopology(this->PrimitiveTopology);
    // p->SetVertexState(this->VertexInput->GetVertexInputInformation());
    // Finally, register the handle with the device
    device->CreatePipeline(this->PipelineHash, p);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::SetBindables(std::vector<vtkWebGPUBindableObject*> b)
{
  if (this->Bindables.size() == b.size())
  {
    auto bindablesIter = this->Bindables.cbegin();
    auto bIter = b.cbegin();
    while (bindablesIter != this->Bindables.cend() && bIter != b.cend())
    {
      // Early exit if they don't match
      if (*bindablesIter != *bIter)
      {
        break;
      }
      ++bindablesIter;
      ++bIter;
    }
    if (bindablesIter == this->Bindables.cend())
    {
      // All the elements matched.
      return;
    }
  }
  // Copy
  this->Bindables.clear();
  this->Bindables = b;
  this->Modified();
}

//-------------------------------------------------------------------------------------------------
std::vector<vtkWebGPUBindableObject*> vtkWebGPUMapperNode::GetBindables()
{
  // Add the bindables at to the vector
  this->Bindables.clear();
  if (this->GetUBO())
  {
    this->Bindables.push_back(this->GetUBO());
  }
  if (this->GetSSBO())
  {
    this->Bindables.push_back(this->GetSSBO());
  }

  for (auto texView : this->TextureViews)
  {
    this->Bindables.push_back(texView);
    auto sampler = texView->GetSampler();
    if (sampler)
    {
      this->Bindables.push_back(sampler);
    }
  }
  return this->Bindables;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::ComputePipelineHash() {}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
