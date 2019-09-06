/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkHierarchicalPolyDataMapper.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkHierarchicalPolyDataMapper);

vtkHierarchicalPolyDataMapper::vtkHierarchicalPolyDataMapper() = default;

vtkHierarchicalPolyDataMapper::~vtkHierarchicalPolyDataMapper() = default;

void vtkHierarchicalPolyDataMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
