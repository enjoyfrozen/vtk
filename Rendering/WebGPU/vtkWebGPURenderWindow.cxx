/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURenderWindow.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkWebGPURenderWindow.h"

// VTK includes
#include "vtkObjectFactory.h"
#include "vtkWebGPUWindowNode.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWebGPURenderWindow);

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderWindow::vtkWebGPURenderWindow()
{
  this->WindowNode = vtkWebGPUWindowNode::New();
  this->WindowNode->SetRenderable(this);
}

//-------------------------------------------------------------------------------------------------
vtkWebGPURenderWindow::~vtkWebGPURenderWindow()
{
  this->WindowNode->Delete();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{

  this->Superclass::PrintSelf(os, indent);
  if (this->WindowNode)
  {
    os << indent << "WindowNode:\n";
    this->WindowNode->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << indent << "WindowNode: (null)\n";
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderWindow::Render()
{
  // Abort if a previous render is ongoing.
  if (this->InRender)
  {
    return;
  }
  this->Superclass::Render();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderWindow::Start()
{
  if (!this->WindowNode)
  {
    vtkErrorMacro(<< "Start called without an active scene graph");
    return;
  }
  this->Superclass::Start();
  this->WindowNode->TraverseAllPasses();
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderWindow::SetInteractor(vtkRenderWindowInteractor* iren)
{
  if (!this->WindowNode)
  {
    vtkErrorMacro(<< "SetInteractor called without an active scene graph");
    return;
  }
  this->WindowNode->SetInteractor(iren);
  this->Superclass::SetInteractor(iren);
}

//-------------------------------------------------------------------------------------------------
vtkHardwareWindow* vtkWebGPURenderWindow::GetHardwareWindow()
{
  if (!this->WindowNode)
  {
    vtkErrorMacro(<< "GetHardwareWindow called without an active scene graph");
    return nullptr;
  }
  vtkHardwareWindow* win = this->WindowNode->GetHardwareWindow();
  if (!win)
  {
    vtkWarningMacro(
      << "No hardware window could mean one of two things:\n"
      << "1. Scenegraph not initialized. Call Render before accessing the hardware window.\n"
      << "2. VTK could not figure out which platform window to create. Instantiate a suitable "
      << "hardware window and set it using SetHardwareWindow.");
  }
  return win;
}

//-------------------------------------------------------------------------------------------------
void vtkWebGPURenderWindow::SetHardwareWindow(vtkHardwareWindow* win)
{
  if (!this->WindowNode)
  {
    vtkErrorMacro(<< "SetHardwareWindow called without an active scene graph");
    return;
  }

  this->WindowNode->SetHardwareWindow(win);
}

VTK_ABI_NAMESPACE_END
