/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkEventForwarderCommand.h"
#include "vtkObject.h"

//----------------------------------------------------------------
vtkEventForwarderCommand::vtkEventForwarderCommand()
{
  this->Target = nullptr;
}

//----------------------------------------------------------------
void vtkEventForwarderCommand::Execute(vtkObject *,
                                       unsigned long event,
                                       void *call_data)
{
  if (this->Target)
  {
    this->Target->InvokeEvent(event, call_data);
  }
}

