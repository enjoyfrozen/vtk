/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkStaticCellLinks.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkStaticCellLinks);

//----------------------------------------------------------------------------
vtkStaticCellLinks::vtkStaticCellLinks()
{
  this->Type = vtkAbstractCellLinks::STATIC_CELL_LINKS_IDTYPE;
  this->Impl = new vtkStaticCellLinksTemplate<vtkIdType>;
}

//----------------------------------------------------------------------------
vtkStaticCellLinks::~vtkStaticCellLinks()
{
  delete this->Impl;
}

//----------------------------------------------------------------------------
void vtkStaticCellLinks::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Implementation: " << this->Impl << "\n";
}
