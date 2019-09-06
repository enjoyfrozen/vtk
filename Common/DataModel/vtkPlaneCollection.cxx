/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPlaneCollection.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPlaneCollection);

vtkPlane *vtkPlaneCollection::GetNextPlane(
  vtkCollectionSimpleIterator &cookie)
{
  return static_cast<vtkPlane *>(this->GetNextItemAsObject(cookie));
}
