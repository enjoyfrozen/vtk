/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURendererNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPURendererNode.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkLightCollection.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkWebGPUBindGroup.h"
#include "vtkWebGPURenderPassEncoder.h"
#include "vtkWebGPUStorageBuffer.h"
#include "vtkWebGPUUniformBuffer.h"
#include "vtkWindowNode.h"
#include "vtk_wgpu.h"

// STL includes
#include <vector>

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPURendererNode);
vtkCxxSetObjectMacro(vtkWebGPURendererNode, RenderEncoder, vtkWebGPURenderPassEncoder);

//-------------------------------------------------------------------------------------------------
vtkWebGPURendererNode::vtkWebGPURendererNode()
{
  this->UBO = vtkWebGPUUniformBuffer::New();
  this->UBO->SetLabel("RendererUBO");
  this->SSBO = vtkWebGPUStorageBuffer::New();
  this->SSBO->SetLabel("RendererLightSSBO");
  std::vector<vtkWebGPUBindableObject*> bindables(2);
  bindables.at(0) = this->UBO;
  bindables.at(1) = this->SSBO;
  this->BindGroup = vtkWebGPUBindGroup::New();
  this->BindGroup->SetBindables(bindables);
}

//-------------------------------------------------------------------------------------------------
vtkWebGPURendererNode::~vtkWebGPURendererNode()
{
  this->SetRenderEncoder(nullptr);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::Build(bool prepass)
{
  if (prepass)
  {
    if (!this->GetRenderable())
    {
      return;
    }

    auto ren = vtkRenderer::SafeDownCast(this->GetRenderable());
    this->Camera = vtkCamera::SafeDownCast(ren->GetActiveCamera());
    this->UpdateLights();
    this->PrepareNodes();
    this->AddMissingNode(this->Camera);
    this->AddMissingNodes(ren->GetViewProps());
    this->RemoveUnusedNodes();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::OpaquePass(bool prepass)
{
  if (!this->RenderEncoder)
  {
    return;
  }
  if (prepass)
  {
    this->RenderEncoder->Begin();
  }
  else
  {
    this->ScissorAndViewport(this->RenderEncoder);
    this->Clear();
    this->RenderEncoder->End();
  }
}

//-------------------------------------------------------------------------------------------------
int vtkWebGPURendererNode::UpdateLights()
{
  vtkRenderer* ren = vtkRenderer::SafeDownCast(this->GetRenderable());
  if (!ren)
  {
    return 0;
  }
  // Count the number of lights that are on
  int count = 0;
  vtkCollectionSimpleIterator iter;
  vtkLightCollection* lights = ren->GetLights();
  lights->InitTraversal(iter);
  vtkLight* l = nullptr;
  while ((l = vtkLight::SafeDownCast(lights->GetNextLight(iter))))
  {
    if (l->GetSwitch())
    {
      count++;
    }
  }

  if (count < 1)
  {
    // If no light is on, create a light
    if (ren->GetAutomaticLightCreation())
    {
      vtkDebugMacro(<< "No lights are on, creating one.");
      ren->CreateLight();
      count++;
    }
  }

  return count;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::UpdateStabilizedMatrix()
{
  if (!this->Camera)
  {
    return;
  }
  double* clipRange = this->Camera->GetClippingRange();
  double* pos = this->Camera->GetPosition();
  double* dop = this->Camera->GetDirectionOfProjection();
  double center[3] = { 0, 0, 0 };
  double offset[3] = { dop[0], dop[1], dop[2] };
  vtkMath::MultiplyScalar(offset, 0.5 * (clipRange[0] + clipRange[1]));
  vtkMath::Add(pos, offset, center);
  vtkMath::Subtract(center, this->StabilizedCenter, offset);
  double length = std::sqrt(offset[0] * offset[0] + offset[1] + offset[1] + offset[2] * offset[2]);
  if (length / (clipRange[1] - clipRange[0]) > this->RecenterThreshold)
  {
    this->StabilizedCenter[0] = center[0];
    this->StabilizedCenter[1] = center[1];
    this->StabilizedCenter[2] = center[2];
    this->Modified();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::ScissorAndViewport(vtkWebGPURenderPassEncoder* encoder)
{
  if (!encoder)
  {
    return;
  }

  vtkRenderer* ren = vtkRenderer::SafeDownCast(this->GetRenderable());
  if (!ren)
  {
    return;
  }

  // Get the Y-inverted origin
  int height, width, topLeftX, topLeftY;
  ren->GetTiledSizeAndOrigin(&width, &height, &topLeftX, &topLeftY);
  WGPURenderPassEncoder enc = static_cast<WGPURenderPassEncoder>(encoder->GetHandle());
  if (!enc)
  {
    return;
  }

  wgpuRenderPassEncoderSetViewport(enc, topLeftX, topLeftY, width, height, 0.0, 1.0);
  wgpuRenderPassEncoderSetScissorRect(enc, topLeftX, topLeftY, width, height);
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::GetYInvertedTiledSizeAndOrigin(
  int* width, int* height, int* topLeftX, int* topLeftY)
{
  vtkRenderer* ren = vtkRenderer::SafeDownCast(this->GetRenderable());
  if (!ren)
  {
    return;
  }
  ren->GetTiledSizeAndOrigin(width, height, topLeftX, topLeftY);
  int* size = vtkWindowNode::SafeDownCast(this->GetParent())->GetSize();
  *topLeftY = size[1] - *height - *topLeftY;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::Clear()
{
  vtkRenderer* ren = vtkRenderer::SafeDownCast(this->GetRenderable());
  if (!ren || ren->GetPreserveColorBuffer())
  {
    return;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::ActivateBindGroup()
{
  if (!this->RenderEncoder)
  {
    return;
  }
  this->RenderEncoder->ActivateBindGroup(this->BindGroup);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
