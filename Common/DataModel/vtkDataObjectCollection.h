/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkDataObjectCollection
 * @brief   maintain an unordered list of data objects
 *
 * vtkDataObjectCollection is an object that creates and manipulates ordered
 * lists of data objects. See also vtkCollection and subclasses.
*/

#ifndef vtkDataObjectCollection_h
#define vtkDataObjectCollection_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkCollection.h"

#include "vtkDataObject.h" // Needed for inline methods

class VTKCOMMONDATAMODEL_EXPORT vtkDataObjectCollection : public vtkCollection
{
public:
  static vtkDataObjectCollection *New();
  vtkTypeMacro(vtkDataObjectCollection,vtkCollection);

  /**
   * Add a data object to the bottom of the list.
   */
  void AddItem(vtkDataObject *ds)
  {
      this->vtkCollection::AddItem(ds);
  }

  /**
   * Get the next data object in the list.
   */
  vtkDataObject *GetNextItem()
  {
      return static_cast<vtkDataObject *>(this->GetNextItemAsObject());
  }

  /**
   * Get the ith data object in the list.
   */
  vtkDataObject *GetItem(int i)
  {
      return static_cast<vtkDataObject *>(this->GetItemAsObject(i));
  }

  /**
   * Reentrant safe way to get an object in a collection. Just pass the
   * same cookie back and forth.
   */
  vtkDataObject *GetNextDataObject(vtkCollectionSimpleIterator &cookie)
  {
      return static_cast<vtkDataObject *>(this->GetNextItemAsObject(cookie));
  }

protected:
  vtkDataObjectCollection() {}
  ~vtkDataObjectCollection() override {}


private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

private:
  vtkDataObjectCollection(const vtkDataObjectCollection&) = delete;
  void operator=(const vtkDataObjectCollection&) = delete;
};


#endif
// VTK-HeaderTest-Exclude: vtkDataObjectCollection.h
