/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkSMPProgressObserver.h"

#include "vtkCommand.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkSMPProgressObserver);

vtkSMPProgressObserver::vtkSMPProgressObserver() = default;

vtkSMPProgressObserver::~vtkSMPProgressObserver() = default;

void vtkSMPProgressObserver::UpdateProgress(double progress)
{
  vtkProgressObserver* observer = this->Observers.Local();
  observer->UpdateProgress(progress);
}

void vtkSMPProgressObserver::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
