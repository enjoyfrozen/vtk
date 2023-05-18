/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHardwareWindow.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkHardwareWindow.h"

#include "vtkObjectFactory.h"

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_BEGIN
vtkObjectFactoryNewMacro(vtkHardwareWindow);

vtkHardwareWindow::vtkHardwareWindow()
{
  this->Borders = true;
#ifdef VTK_DEFAULT_RENDER_WINDOW_OFFSCREEN
  this->ShowWindow = false;
  this->UseOffScreenBuffers = true;
#else
  this->ShowWindow = true;
#endif
}

//-------------------------------------------------------------------------------------------------
vtkHardwareWindow::~vtkHardwareWindow()
{
  this->SetInteractor(nullptr);
}

//-------------------------------------------------------------------------------------------------
void vtkHardwareWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Borders: " << this->Borders << "\n";
}

//------------------------------------------------------------------------------
// Set the interactor that will work with this hardware window.
void vtkHardwareWindow::SetInteractor(vtkRenderWindowInteractor* rwi)
{
  this->Interactor = rwi;
  if (this->Interactor->GetHardwareWindow() != this)
  {
    this->Interactor->SetHardwareWindow(this);
  }
}

//------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
