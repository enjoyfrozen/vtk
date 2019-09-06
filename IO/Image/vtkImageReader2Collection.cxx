/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkImageReader2Collection.h"

#include "vtkObjectFactory.h"
#include "vtkImageReader2.h"

vtkStandardNewMacro(vtkImageReader2Collection);

void vtkImageReader2Collection::AddItem(vtkImageReader2 *f)
{
  this->vtkCollection::AddItem(f);
}

vtkImageReader2 *vtkImageReader2Collection::GetNextItem()
{
  return static_cast<vtkImageReader2*>(this->GetNextItemAsObject());
}

vtkImageReader2 *vtkImageReader2Collection::GetNextImageReader2(
  vtkCollectionSimpleIterator &cookie)
{
  return static_cast<vtkImageReader2 *>(this->GetNextItemAsObject(cookie));
}

//----------------------------------------------------------------------------
void vtkImageReader2Collection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
