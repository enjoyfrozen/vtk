/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkContextKeyEvent.h"

#include "vtkRenderWindowInteractor.h"

#include <cassert>

vtkContextKeyEvent::vtkContextKeyEvent()
{
}

void vtkContextKeyEvent::SetInteractor(vtkRenderWindowInteractor *interactor)
{
  this->Interactor = interactor;
}

vtkRenderWindowInteractor* vtkContextKeyEvent::GetInteractor() const
{
  return this->Interactor;
}

char vtkContextKeyEvent::GetKeyCode() const
{
  if (this->Interactor)
  {
    return this->Interactor->GetKeyCode();
  }
  else
  {
    // This should never happen, perhaps there is a better return value?
    return 0;
  }
}
