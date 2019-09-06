/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkRenderPassCollection.h"
#include "vtkObjectFactory.h"
#include "vtkRenderPass.h"

vtkStandardNewMacro(vtkRenderPassCollection);

// ----------------------------------------------------------------------------
// Description:
// Reentrant safe way to get an object in a collection. Just pass the
// same cookie back and forth.
vtkRenderPass *vtkRenderPassCollection::GetNextRenderPass(
vtkCollectionSimpleIterator &cookie)
{
  return static_cast<vtkRenderPass *>(this->GetNextItemAsObject(cookie));
}

// ----------------------------------------------------------------------------
vtkRenderPassCollection::vtkRenderPassCollection() = default;

// ----------------------------------------------------------------------------
vtkRenderPassCollection::~vtkRenderPassCollection() = default;

// ----------------------------------------------------------------------------
// hide the standard AddItem from the user and the compiler.
void vtkRenderPassCollection::AddItem(vtkObject *o)
{
  this->vtkCollection::AddItem(o);
}

// ----------------------------------------------------------------------------
void vtkRenderPassCollection::AddItem(vtkRenderPass *a)
{
  this->vtkCollection::AddItem(a);
}

// ----------------------------------------------------------------------------
vtkRenderPass *vtkRenderPassCollection::GetNextRenderPass()
{
  return static_cast<vtkRenderPass *>(this->GetNextItemAsObject());
}

// ----------------------------------------------------------------------------
vtkRenderPass *vtkRenderPassCollection::GetLastRenderPass()
{
  return (this->Bottom) ?
    static_cast<vtkRenderPass *>(this->Bottom->Item) : nullptr;
}

// ----------------------------------------------------------------------------
void vtkRenderPassCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
