/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkArrayIterator.h"

#include "vtkObjectFactory.h"

//-----------------------------------------------------------------------------
vtkArrayIterator::vtkArrayIterator() = default;

//-----------------------------------------------------------------------------
vtkArrayIterator::~vtkArrayIterator() = default;

//-----------------------------------------------------------------------------
void vtkArrayIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
