/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAffineRepresentation.h"
#include "vtkTransform.h"
#include "vtkObjectFactory.h"



//----------------------------------------------------------------------
vtkAffineRepresentation::vtkAffineRepresentation()
{
  this->InteractionState = vtkAffineRepresentation::Outside;
  this->Tolerance = 15;
  this->Transform = vtkTransform::New();
}

//----------------------------------------------------------------------
vtkAffineRepresentation::~vtkAffineRepresentation()
{
  this->Transform->Delete();
}

//----------------------------------------------------------------------
void vtkAffineRepresentation::ShallowCopy(vtkProp *prop)
{
  vtkAffineRepresentation *rep = vtkAffineRepresentation::SafeDownCast(prop);
  if ( rep )
  {
    this->SetTolerance(rep->GetTolerance());
  }
  this->Superclass::ShallowCopy(prop);
}

//----------------------------------------------------------------------
void vtkAffineRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Tolerance: " << this->Tolerance << "\n";
}
