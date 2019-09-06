/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkInteractorStyleSwitchBase.h"

#include "vtkObjectFactory.h"

// This is largely here to confirm the approach works, and will be replaced
// with standard factory override logic in the modularized source tree.
//----------------------------------------------------------------------------
vtkObjectFactoryNewMacro(vtkInteractorStyleSwitchBase)

//----------------------------------------------------------------------------
vtkInteractorStyleSwitchBase::vtkInteractorStyleSwitchBase() = default;

//----------------------------------------------------------------------------
vtkInteractorStyleSwitchBase::~vtkInteractorStyleSwitchBase() = default;

//----------------------------------------------------------------------------
vtkRenderWindowInteractor* vtkInteractorStyleSwitchBase::GetInteractor()
{
  static bool warned = false;
  if (!warned &&
      strcmp(this->GetClassName(), "vtkInteractorStyleSwitchBase") == 0)
  {
    vtkWarningMacro(
      "Warning: Link to vtkInteractionStyle for default style selection.");
    warned = true;
  }
  return nullptr;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleSwitchBase::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
