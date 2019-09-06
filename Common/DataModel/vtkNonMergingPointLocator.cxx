/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkNonMergingPointLocator.h"

#include "vtkPoints.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro( vtkNonMergingPointLocator );

//----------------------------------------------------------------------------
int vtkNonMergingPointLocator::InsertUniquePoint
  ( const double x[3], vtkIdType & ptId )
{
  ptId = this->Points->InsertNextPoint( x );
  return 1;
}

//----------------------------------------------------------------------------
void vtkNonMergingPointLocator::PrintSelf( ostream & os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os,indent );
}
