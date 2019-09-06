/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkGraphEdge.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkGraphEdge);
//----------------------------------------------------------------------------
vtkGraphEdge::vtkGraphEdge()
{
  this->Source = 0;
  this->Target = 0;
  this->Id = 0;
}

//----------------------------------------------------------------------------
vtkGraphEdge::~vtkGraphEdge() = default;

//----------------------------------------------------------------------------
void vtkGraphEdge::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Source: " << this->Source << endl;
  os << indent << "Target: " << this->Target << endl;
  os << indent << "Id: " << this->Id << endl;
}
