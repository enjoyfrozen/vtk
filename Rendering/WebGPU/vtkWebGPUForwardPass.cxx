/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUForwardPass.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUForwardPass.h"

#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkWebGPURendererNode.h"
#include "vtkWebGPUViewNode.h"
#include "vtkWebGPUWindowNode.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUForwardPass);

//-------------------------------------------------------------------------------------------------
vtkWebGPUForwardPass::vtkWebGPUForwardPass() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUForwardPass::~vtkWebGPUForwardPass() {}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUForwardPass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "OpaqueActorCount = " << this->OpaqueActorCount << endl;
  os << indent << "TranslucentActorCount = " << this->TranslucentActorCount << endl;
  os << indent << "VolumeCount = " << this->VolumeCount << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUForwardPass::Traverse(vtkViewNode* vn, vtkRenderPass* parent)
{
  vtkWebGPUWindowNode* wn = vtkWebGPUWindowNode::SafeDownCast(vn);
  if (!wn)
  {
    vtkErrorMacro(<< "Forward pass expects to be the rendering controller for vtkWebGPUWindowNode "
                  << "and its children.");
    return;
  }

  this->CurrentParent = parent;

  // Render the delegates in order
  // Build pass - Gathers the information on scenegraph nodes and assigns renderables
  vn->Traverse(vtkViewNode::build);

  // Query pass - accumulate types of actors
  int numLayers = vtkRenderWindow::SafeDownCast(wn->GetRenderable())->GetNumberOfLayers();
  // iterate over renderers
  auto renderers = wn->GetChildren();
  for (int i = 0; i < numLayers; ++i)
  {
    for (auto rn : renderers)
    {
      vtkWebGPURendererNode* renNode = vtkWebGPURendererNode::SafeDownCast(rn);
      if (!renNode)
      {
        continue;
      }
      vtkRenderer* ren = vtkRenderer::SafeDownCast(renNode->GetRenderable());
      if (ren->GetDraw() && ren->GetLayer() == i)
      {
        // reset the counts before the query pass
        this->OpaqueActorCount = 0;
        this->TranslucentActorCount = 0;
        this->VolumeCount = 0;
        renNode->Traverse(vtkWebGPUViewNode::query);

        // Opaque pass - draw opaque actors
        // Translucent pass - translucent actors
        // Volume pass - volumes
        // Final pass - Blit the result into the swap chain
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUForwardPass::IncrementOpaqueActorCount()
{
  this->OpaqueActorCount++;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUForwardPass::IncrementTranslucentActorCount()
{
  this->TranslucentActorCount++;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUForwardPass::IncrementVolumeCount()
{
  this->VolumeCount++;
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
