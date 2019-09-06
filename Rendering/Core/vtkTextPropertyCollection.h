/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTextPropertyCollection
 * @brief   an ordered list of vtkTextProperty objects.
 *
 * vtkTextPropertyCollection represents and provides methods to manipulate a
 * list of TextProperty objects. The list is ordered and
 * duplicate entries are not prevented.
 * @sa
 * vtkTextProperty vtkCollection
*/

#ifndef vtkTextPropertyCollection_h
#define vtkTextPropertyCollection_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkCollection.h"
#include "vtkTextProperty.h" // for inline functions

class VTKRENDERINGCORE_EXPORT vtkTextPropertyCollection : public vtkCollection
{
 public:
  static vtkTextPropertyCollection *New();
  vtkTypeMacro(vtkTextPropertyCollection, vtkCollection)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Add a vtkTextProperty to the bottom of the list.
   */
  void AddItem(vtkTextProperty *a);

  /**
   * Get the next vtkTextProperty in the list.
   */
  vtkTextProperty *GetNextItem();

  /**
   * Get the vtkTextProperty at the specified index.
   */
  vtkTextProperty *GetItem(int idx);

  /**
   * Get the last TextProperty in the list.
   */
  vtkTextProperty *GetLastItem();

  /**
   * Reentrant safe way to get an object in a collection. Just pass the
   * same cookie back and forth.
   */
  vtkTextProperty *GetNextTextProperty(vtkCollectionSimpleIterator &cookie);

protected:
  vtkTextPropertyCollection();
  ~vtkTextPropertyCollection() override;

private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o);

private:
  vtkTextPropertyCollection(const vtkTextPropertyCollection&) = delete;
  void operator=(const vtkTextPropertyCollection&) = delete;
};

inline void vtkTextPropertyCollection::AddItem(vtkTextProperty *a)
{
  this->vtkCollection::AddItem(a);
}

inline vtkTextProperty *vtkTextPropertyCollection::GetNextItem()
{
  return static_cast<vtkTextProperty *>(this->GetNextItemAsObject());
}

inline vtkTextProperty *vtkTextPropertyCollection::GetItem(int idx)
{
  return static_cast<vtkTextProperty *>(this->GetItemAsObject(idx));
}

inline vtkTextProperty *vtkTextPropertyCollection::GetLastItem()
{
  if ( this->Bottom == nullptr )
  {
    return nullptr;
  }
  else
  {
    return static_cast<vtkTextProperty *>(this->Bottom->Item);
  }
}

inline vtkTextProperty *
vtkTextPropertyCollection::GetNextTextProperty(vtkCollectionSimpleIterator &it)
{
  return static_cast<vtkTextProperty *>(this->GetNextItemAsObject(it));
}

inline void vtkTextPropertyCollection::AddItem(vtkObject *o)
{
  this->vtkCollection::AddItem(o);
}

#endif
