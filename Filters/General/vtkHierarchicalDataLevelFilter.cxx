/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkHierarchicalDataLevelFilter.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkHierarchicalDataLevelFilter);

// Construct object with PointIds and CellIds on; and ids being generated
// as scalars.
vtkHierarchicalDataLevelFilter::vtkHierarchicalDataLevelFilter() = default;

vtkHierarchicalDataLevelFilter::~vtkHierarchicalDataLevelFilter() = default;

void vtkHierarchicalDataLevelFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
