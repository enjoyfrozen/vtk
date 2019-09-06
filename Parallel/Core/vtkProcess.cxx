/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkProcess.h"
#include "vtkMultiProcessController.h"


// ----------------------------------------------------------------------------
vtkProcess::vtkProcess()
{
  this->Controller=nullptr;
  this->ReturnValue=0;
}

// ----------------------------------------------------------------------------
vtkMultiProcessController *vtkProcess::GetController()
{
  return this->Controller;
}

// ----------------------------------------------------------------------------
void vtkProcess::SetController(vtkMultiProcessController *aController)
{
  this->Controller=aController;
}

// ----------------------------------------------------------------------------
int vtkProcess::GetReturnValue()
{
  return this->ReturnValue;
}

//----------------------------------------------------------------------------
void vtkProcess::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "ReturnValue: " << this->ReturnValue << endl;
  os << indent << "Controller: ";
  if(this->Controller)
  {
    os << endl;
    this->Controller->PrintSelf(os, indent.GetNextIndent());
  }
  else
  {
    os << "(none)" << endl;
  }
}
