/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkNonOverlappingAMR.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkNonOverlappingAMR);

//------------------------------------------------------------------------------
vtkNonOverlappingAMR::vtkNonOverlappingAMR() = default;

//------------------------------------------------------------------------------
vtkNonOverlappingAMR::~vtkNonOverlappingAMR() = default;

//------------------------------------------------------------------------------
void vtkNonOverlappingAMR::PrintSelf(ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf(os,indent);
}
