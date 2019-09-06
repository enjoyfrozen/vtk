/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkHierarchicalDataSetGeometryFilter.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkHierarchicalDataSetGeometryFilter);
//-----------------------------------------------------------------------------
vtkHierarchicalDataSetGeometryFilter::vtkHierarchicalDataSetGeometryFilter() = default;

//-----------------------------------------------------------------------------
vtkHierarchicalDataSetGeometryFilter::~vtkHierarchicalDataSetGeometryFilter() = default;

//-----------------------------------------------------------------------------
void vtkHierarchicalDataSetGeometryFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

