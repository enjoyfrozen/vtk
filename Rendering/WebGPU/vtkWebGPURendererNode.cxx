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
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkWebGPURenderPassEncoder.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPURendererNode);
vtkCxxSetObjectMacro(vtkWebGPURendererNode, RenderEncoder, vtkWebGPURenderPassEncoder);

//-------------------------------------------------------------------------------------------------
vtkWebGPURendererNode::vtkWebGPURendererNode() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPURendererNode::~vtkWebGPURendererNode() {}

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
void vtkWebGPURendererNode::UpdateLights() {}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURendererNode::UpdateStabilizedMatrix() {}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
