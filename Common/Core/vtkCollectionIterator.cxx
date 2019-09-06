/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkCollectionIterator.h"
#include "vtkObjectFactory.h"
#include "vtkCollection.h"

vtkStandardNewMacro(vtkCollectionIterator);

//----------------------------------------------------------------------------
vtkCollectionIterator::vtkCollectionIterator()
{
  this->Element = nullptr;
  this->Collection = nullptr;
}

//----------------------------------------------------------------------------
vtkCollectionIterator::~vtkCollectionIterator()
{
  this->SetCollection(nullptr);
}

//----------------------------------------------------------------------------
void vtkCollectionIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if(this->Collection)
  {
    os << indent << "Collection: " << this->Collection << "\n";
  }
  else
  {
    os << indent << "Collection: (none)\n";
  }
}

//----------------------------------------------------------------------------
void vtkCollectionIterator::SetCollection(vtkCollection* collection)
{
  vtkSetObjectBodyMacro(Collection, vtkCollection, collection);
  this->GoToFirstItem();
}

//----------------------------------------------------------------------------
void vtkCollectionIterator::GoToFirstItem()
{
  if(this->Collection)
  {
    this->Element = this->Collection->Top;
  }
  else
  {
    this->Element = nullptr;
  }
}

//----------------------------------------------------------------------------
void vtkCollectionIterator::GoToNextItem()
{
  if(this->Element)
  {
    this->Element = this->Element->Next;
  }
}

//----------------------------------------------------------------------------
int vtkCollectionIterator::IsDoneWithTraversal()
{
  return (this->Element? 0:1);
}

//----------------------------------------------------------------------------
vtkObject* vtkCollectionIterator::GetCurrentObject()
{
  if(this->Element)
  {
    return this->Element->Item;
  }
  return nullptr;
}
