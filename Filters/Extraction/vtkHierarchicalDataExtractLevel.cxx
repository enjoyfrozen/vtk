/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkHierarchicalDataExtractLevel.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkHierarchicalDataExtractLevel);

//----------------------------------------------------------------------------
vtkHierarchicalDataExtractLevel::vtkHierarchicalDataExtractLevel() = default;

//----------------------------------------------------------------------------
vtkHierarchicalDataExtractLevel::~vtkHierarchicalDataExtractLevel() = default;

//----------------------------------------------------------------------------
void vtkHierarchicalDataExtractLevel::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
