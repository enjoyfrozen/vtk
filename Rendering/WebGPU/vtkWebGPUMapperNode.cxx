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
#include "vtkWebGPURenderPassEncoder.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUMapperNode);

//-------------------------------------------------------------------------------------------------
vtkWebGPUMapperNode::vtkWebGPUMapperNode() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUMapperNode::~vtkWebGPUMapperNode() {}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-><< endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::PrepareAndDraw(vtkWebGPURenderPassEncoder* encoder)
{
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::PrepareToDraw(vtkWebGPURenderPassEncoder* encoder)
{
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::Draw(vtkWebGPURenderPassEncoder* encoder)
{
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdateInput()
{
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdateBuffers()
{
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdateBindings()
{
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUMapperNode::UpdatePipeline()
{
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
