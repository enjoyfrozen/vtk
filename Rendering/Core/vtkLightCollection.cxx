/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkLightCollection.h"

#include "vtkObjectFactory.h"
#include "vtkLight.h"

#include <cmath>

vtkStandardNewMacro(vtkLightCollection);

// Add a light to the bottom of the list.
void vtkLightCollection::AddItem(vtkLight *a)
{
  this->vtkCollection::AddItem(a);
}

// Get the next light in the list. nullptr is returned when the collection is
// exhausted.
vtkLight *vtkLightCollection::GetNextItem()
{
  return static_cast<vtkLight *>(this->GetNextItemAsObject());
}

vtkLight *vtkLightCollection::GetNextLight(
  vtkCollectionSimpleIterator &cookie)
{
  return static_cast<vtkLight *>(this->GetNextItemAsObject(cookie));
}

//----------------------------------------------------------------------------
void vtkLightCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
