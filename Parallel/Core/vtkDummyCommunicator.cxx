/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkDummyCommunicator.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkDummyCommunicator);

//-----------------------------------------------------------------------------
vtkDummyCommunicator::vtkDummyCommunicator()
{
  this->MaximumNumberOfProcesses = 1;
}

vtkDummyCommunicator::~vtkDummyCommunicator() = default;

void vtkDummyCommunicator::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
