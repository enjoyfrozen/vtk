/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkReaderAlgorithm.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkReaderAlgorithm::vtkReaderAlgorithm()
{
  // by default assume filters have one input and one output
  // subclasses that deviate should modify this setting
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkReaderAlgorithm::~vtkReaderAlgorithm() = default;

//----------------------------------------------------------------------------
void vtkReaderAlgorithm::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
