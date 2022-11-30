/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleSwitchBase.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkInteractorStyleSwitchBase.h"

#include "vtkObjectFactory.h"

#define vtkForwardSetMacro(function, type)                    \
  void vtkInteractorStyleSwitchBase::Set##function(type arg)  \
  {                                                           \
    this->Superclass::Set##function(arg);                     \
    for (auto style : this->Styles)                           \
    {                                                         \
      if (style)                                              \
      {                                                       \
        style->Set##function(arg);                            \
      }                                                       \
    }                                                         \
  }

// This is largely here to confirm the approach works, and will be replaced
// with standard factory override logic in the modularized source tree.
//------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_BEGIN
vtkObjectFactoryNewMacro(vtkInteractorStyleSwitchBase);

//------------------------------------------------------------------------------
vtkInteractorStyleSwitchBase::vtkInteractorStyleSwitchBase() = default;

//------------------------------------------------------------------------------
vtkInteractorStyleSwitchBase::~vtkInteractorStyleSwitchBase() = default;

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitchBase::SetInteractor(vtkRenderWindowInteractor* interactor)
{
  if (interactor == this->Interactor)
  {
    return;
  }

  this->Interactor = interactor;

  if (!this->Styles.empty())
  {
    if (auto newStyle = this->Styles[this->CurrentStyleId])
    {
      newStyle->SetInteractor(this->Interactor);
    }
  }

  this->Modified();
}

//------------------------------------------------------------------------------
vtkRenderWindowInteractor* vtkInteractorStyleSwitchBase::GetInteractor()
{
  static bool warned = false;
  if (!warned && strcmp(this->GetClassName(), "vtkInteractorStyleSwitchBase") == 0)
  {
    vtkWarningMacro("Warning: Link to vtkInteractionStyle for default style selection.");
    warned = true;
  }
  return nullptr;
}

//------------------------------------------------------------------------------
std::size_t vtkInteractorStyleSwitchBase::AddStyle(vtkInteractorStyle* style)
{
  this->Styles.push_back(style);
  this->Modified();
  return this->Styles.size() - 1;
}

//------------------------------------------------------------------------------
std::size_t vtkInteractorStyleSwitchBase::GetNumberOfStyles()
{
  return this->Styles.size();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitchBase::SetCurrentStyle(std::size_t id)
{
  if (id >= this->Styles.size())
  {
    vtkErrorMacro("Provided interactor style index is out of range");
    return;
  }

  if (id == this->CurrentStyleId)
  {
    return;
  }

  if (auto oldStyle = this->Styles[this->CurrentStyleId])
  {
    oldStyle->SetInteractor(nullptr);
  }

  this->CurrentStyleId = id;

  if (auto newStyle = this->Styles[this->CurrentStyleId])
  {
    newStyle->SetInteractor(this->Interactor);
    newStyle->SetTDxStyle(this->TDxStyle);
  }

  this->Modified();
}

//------------------------------------------------------------------------------
vtkInteractorStyle* vtkInteractorStyleSwitchBase::GetCurrentStyle()
{
  if (this->Styles.empty())
  {
    return nullptr;
  }

  return this->Styles[this->CurrentStyleId];
}

//------------------------------------------------------------------------------
vtkForwardSetMacro(DefaultRenderer, vtkRenderer*);
vtkForwardSetMacro(CurrentRenderer, vtkRenderer*);
vtkForwardSetMacro(AutoAdjustCameraClippingRange, vtkTypeBool);

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitchBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  auto currentStyle = this->GetCurrentStyle();
  os << indent << "CurrentStyle " << currentStyle << "\n";
  if (currentStyle)
  {
    vtkIndent next_indent = indent.GetNextIndent();
    os << next_indent << currentStyle->GetClassName() << "\n";
    currentStyle->PrintSelf(os, indent.GetNextIndent());
  }
}
VTK_ABI_NAMESPACE_END
