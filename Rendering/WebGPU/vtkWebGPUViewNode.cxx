/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUViewNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPUViewNode.h"

#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUViewNode);

//-------------------------------------------------------------------------------------------------
vtkWebGPUViewNode::vtkWebGPUViewNode() {}

//-------------------------------------------------------------------------------------------------
vtkWebGPUViewNode::~vtkWebGPUViewNode() {}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUViewNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  // os << indent << " = " << this-> << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPUViewNode::Apply(int operation, bool prepass)
{
  switch (operation)
  {
    case vtkWebGPUViewNode::query:
      this->Query(prepass);
      break;
    case vtkWebGPUViewNode::opaquePass:
      this->OpaquePass(prepass);
      break;
    default:
      this->Superclass::Apply(operation, prepass);
      break;
  }
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
