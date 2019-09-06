/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPolyDataContourLineInterpolator.h"

#include "vtkObjectFactory.h"
#include "vtkContourRepresentation.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkPolyDataCollection.h"


//----------------------------------------------------------------------
vtkPolyDataContourLineInterpolator::vtkPolyDataContourLineInterpolator()
{
  this->Polys = vtkPolyDataCollection::New();
}

//----------------------------------------------------------------------
vtkPolyDataContourLineInterpolator::~vtkPolyDataContourLineInterpolator()
{
  this->Polys->Delete();
}

//----------------------------------------------------------------------
void vtkPolyDataContourLineInterpolator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Polys: \n";
  this->Polys->PrintSelf(os,indent.GetNextIndent());

}
