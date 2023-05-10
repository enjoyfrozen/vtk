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
#include "vtkWebGPURendererNode.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUOpaquePass);

//-------------------------------------------------------------------------------------------------
vtkWebGPUOpaquePass::vtkWebGPUOpaquePass() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUOpaquePass::~vtkWebGPUOpaquePass() {}

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
  // TODO: implementation
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
