/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkReferenceCount.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkReferenceCount);

vtkReferenceCount::vtkReferenceCount() = default;

vtkReferenceCount::~vtkReferenceCount() = default;

void vtkReferenceCount::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
