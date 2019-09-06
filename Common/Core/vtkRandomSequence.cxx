/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkRandomSequence.h"

#include <cassert>


// ----------------------------------------------------------------------------
vtkRandomSequence::vtkRandomSequence() = default;

// ----------------------------------------------------------------------------
vtkRandomSequence::~vtkRandomSequence() = default;

// ----------------------------------------------------------------------------
void vtkRandomSequence::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
