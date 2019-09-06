/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkCallbackCommand.h"

//----------------------------------------------------------------
vtkCallbackCommand::vtkCallbackCommand()
{
  this->ClientData = nullptr;
  this->Callback = nullptr;
  this->ClientDataDeleteCallback = nullptr;
  this->AbortFlagOnExecute = 0;
}

//----------------------------------------------------------------
vtkCallbackCommand::~vtkCallbackCommand()
{
  if (this->ClientDataDeleteCallback)
  {
    this->ClientDataDeleteCallback(this->ClientData);
  }
}

//----------------------------------------------------------------
void vtkCallbackCommand::Execute(vtkObject *caller, unsigned long event,
                                 void *callData)
{
  if (this->Callback)
  {
    this->Callback(caller, event, this->ClientData, callData);
    if (this->AbortFlagOnExecute)
    {
      this->AbortFlagOn();
    }
  }
}

