/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPolyDataCollection.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPolyDataCollection);

//----------------------------------------------------------------------------
void vtkPolyDataCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
