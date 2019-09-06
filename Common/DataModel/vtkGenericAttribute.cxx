/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// .NAME vtkGenericAttribute - Objects that manage some attribute data.
// .SECTION Description

#include "vtkGenericAttribute.h"
#include <cassert>


//---------------------------------------------------------------------------
vtkGenericAttribute::vtkGenericAttribute() = default;

//---------------------------------------------------------------------------
vtkGenericAttribute::~vtkGenericAttribute() = default;

//---------------------------------------------------------------------------
void vtkGenericAttribute::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Name: " << this->GetName() << endl;
  os << indent << "Number of components: " << this->GetNumberOfComponents() << endl;
  os << indent << "Centering: ";

  switch(this->GetCentering())
  {
    case vtkPointCentered:
      os << "on points";
      break;
    case vtkCellCentered:
      os << "on cells";
      break;
    case vtkBoundaryCentered:
      os << "on boundaries";
      break;
    default:
      assert("check: Impossible case" && 0);
      break;
  }
  os << endl;
}
