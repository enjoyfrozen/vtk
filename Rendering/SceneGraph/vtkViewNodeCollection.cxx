/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkViewNodeCollection.h"

#include "vtkCollectionIterator.h"
#include "vtkObjectFactory.h"
#include "vtkViewNode.h"

//============================================================================
vtkStandardNewMacro(vtkViewNodeCollection);

//----------------------------------------------------------------------------
void vtkViewNodeCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkViewNodeCollection::AddItem(vtkViewNode *a)
{
  this->vtkCollection::AddItem(a);
}

//----------------------------------------------------------------------------
vtkViewNode *vtkViewNodeCollection::GetNextItem()
{
  return static_cast<vtkViewNode *>(this->GetNextItemAsObject());
}

//----------------------------------------------------------------------------
vtkViewNode *vtkViewNodeCollection::GetNextViewNode(
  vtkCollectionSimpleIterator &cookie)
{
  return static_cast<vtkViewNode *>(this->GetNextItemAsObject(cookie));
}

//----------------------------------------------------------------------------
bool vtkViewNodeCollection::IsRenderablePresent(vtkObject *obj)
{
  vtkCollectionIterator *it = this->NewIterator();
  it->InitTraversal();
  bool found = false;
  while (!found && !it->IsDoneWithTraversal())
  {
    vtkViewNode *vn = vtkViewNode::SafeDownCast(it->GetCurrentObject());
    if (vn)
    {
      vtkObject *nobj = vn->GetRenderable();
      if (nobj == obj)
      {
        found = true;
      }
      it->GoToNextItem();
    }
  }
  it->Delete();

  return found;
}
