/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkActorCollection.h"

#include "vtkObjectFactory.h"
#include "vtkProperty.h"

vtkStandardNewMacro(vtkActorCollection);

void vtkActorCollection::ApplyProperties(vtkProperty *p)
{
  vtkActor *actor;

  if ( p == nullptr )
  {
    return;
  }

  vtkCollectionSimpleIterator ait;
  for ( this->InitTraversal(ait); (actor=this->GetNextActor(ait)); )
  {
    actor->GetProperty()->DeepCopy(p);
  }
}

//----------------------------------------------------------------------------
void vtkActorCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
