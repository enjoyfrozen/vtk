/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkDataArrayCollectionIterator.h"
#include "vtkObjectFactory.h"
#include "vtkDataArray.h"
#include "vtkDataArrayCollection.h"

vtkStandardNewMacro(vtkDataArrayCollectionIterator);

//----------------------------------------------------------------------------
vtkDataArrayCollectionIterator::vtkDataArrayCollectionIterator() = default;

//----------------------------------------------------------------------------
vtkDataArrayCollectionIterator::~vtkDataArrayCollectionIterator() = default;

//----------------------------------------------------------------------------
void vtkDataArrayCollectionIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkDataArrayCollectionIterator::SetCollection(vtkCollection* c)
{
  if(c)
  {
    this->Superclass::SetCollection(vtkDataArrayCollection::SafeDownCast(c));
    if(!this->Collection)
    {
      vtkErrorMacro("vtkDataArrayCollectionIterator cannot traverse a "
                    << c->GetClassName());
    }
  }
  else
  {
    this->Superclass::SetCollection(nullptr);
  }
}

//----------------------------------------------------------------------------
void vtkDataArrayCollectionIterator::SetCollection(vtkDataArrayCollection* c)
{
  this->Superclass::SetCollection(c);
}

//----------------------------------------------------------------------------
vtkDataArray* vtkDataArrayCollectionIterator::GetDataArray()
{
  return static_cast<vtkDataArray*>(this->GetCurrentObject());
}
