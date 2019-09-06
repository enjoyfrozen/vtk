/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkStructuredPointsCollection.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkStructuredPointsCollection);

//----------------------------------------------------------------------------
void vtkStructuredPointsCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
