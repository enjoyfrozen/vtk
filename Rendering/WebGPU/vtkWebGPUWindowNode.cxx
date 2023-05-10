/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUWindowNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// vtk includes
#include "vtkWebGPUWindowNode.h"

#include "vtkHardwareWindow.h"
#include "vtkObjectFactory.h"
#include "vtkRenderPassCollection.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkWebGPUInstance.h"
#include "vtkWebGPURenderWindow.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkWebGPUWindowNode, Instance, vtkWebGPUInstance);
vtkCxxSetObjectMacro(vtkWebGPUWindowNode, Interactor, vtkRenderWindowInteractor);
vtkCxxSetObjectMacro(vtkWebGPUWindowNode, HardwareWindow, vtkHardwareWindow);

//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPUWindowNode);

//-------------------------------------------------------------------------------------------------
vtkWebGPUWindowNode::vtkWebGPUWindowNode()
{
  this->RenderPasses = vtkRenderPassCollection::New();
}

//-------------------------------------------------------------------------------------------------
vtkWebGPUWindowNode::~vtkWebGPUWindowNode()
{
  this->RenderPasses->Delete();
  this->RenderPasses = nullptr;
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "WebGPUInstance:";
  if (this->Instance)
  {
    os << endl;
    this->Instance->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
  os << indent << "Interactor:";
  if (this->Interactor)
  {
    os << endl;
    this->Interactor->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
  os << indent << "HardwareWindow:";
  if (this->HardwareWindow)
  {
    os << endl;
    this->HardwareWindow->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
  os << indent << "RenderPasses:";
  if (this->RenderPasses)
  {
    os << endl;
    this->RenderPasses->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << " (null)" << endl;
  }
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::SetRenderable(vtkObject* obj)
{
  if (this->Renderable == obj)
  {
    return;
  }

  vtkWebGPURenderWindow* renWin = vtkWebGPURenderWindow::SafeDownCast(obj);
  if (!renWin)
  {
    vtkErrorMacro(<< "Renderable must be of type vtkWebGPURenderWindow or its subclass."
                  << "Instead, found " << obj->GetClassName());
    return;
  }
}

//------------------------------------------------------------------------------------------------
void vtkWebGPUWindowNode::TraverseAllPasses() {}

//------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
