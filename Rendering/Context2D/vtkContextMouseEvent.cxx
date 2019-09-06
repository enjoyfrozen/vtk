/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkRenderWindowInteractor.h" // AIX include order issues.
#include "vtkContextMouseEvent.h"

int vtkContextMouseEvent::GetModifiers() const
{
  int modifier = vtkContextMouseEvent::NO_MODIFIER;
  if (this->Interactor)
  {
    if (this->Interactor->GetAltKey() > 0)
    {
      modifier |= vtkContextMouseEvent::ALT_MODIFIER;
    }
    if (this->Interactor->GetShiftKey() > 0)
    {
      modifier |= vtkContextMouseEvent::SHIFT_MODIFIER;
    }
    if (this->Interactor->GetControlKey() > 0)
    {
      modifier |= vtkContextMouseEvent::CONTROL_MODIFIER;
    }
  }
  return modifier;
}
