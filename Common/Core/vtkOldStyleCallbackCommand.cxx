/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkOldStyleCallbackCommand.h"

#include "vtkSetGet.h"
#include "vtkObject.h"

#include <cstring>
#include <cctype>

//----------------------------------------------------------------
vtkOldStyleCallbackCommand::vtkOldStyleCallbackCommand()
{
  this->ClientData = nullptr;
  this->Callback = nullptr;
  this->ClientDataDeleteCallback = nullptr;
}

vtkOldStyleCallbackCommand::~vtkOldStyleCallbackCommand()
{
  if (this->ClientDataDeleteCallback)
  {
    this->ClientDataDeleteCallback(this->ClientData);
  }
}

void vtkOldStyleCallbackCommand::Execute(vtkObject *,unsigned long, void *)
{
  if (this->Callback)
  {
    this->Callback(this->ClientData);
  }
}
