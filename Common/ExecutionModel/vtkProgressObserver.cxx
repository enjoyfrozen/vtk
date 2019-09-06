/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkProgressObserver.h"

#include "vtkCommand.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkProgressObserver);

vtkProgressObserver::vtkProgressObserver() = default;

vtkProgressObserver::~vtkProgressObserver() = default;

void vtkProgressObserver::UpdateProgress(double amount)
{
  this->Progress = amount;
  this->InvokeEvent(vtkCommand::ProgressEvent,static_cast<void *>(&amount));
}

void vtkProgressObserver::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
