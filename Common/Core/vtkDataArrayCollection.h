/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkDataArrayCollection
 * @brief   maintain an ordered list of dataarray objects
 *
 * vtkDataArrayCollection is an object that creates and manipulates lists of
 * datasets. See also vtkCollection and subclasses.
*/

#ifndef vtkDataArrayCollection_h
#define vtkDataArrayCollection_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkCollection.h"

#include "vtkDataArray.h" // Needed for inline methods

class VTKCOMMONCORE_EXPORT vtkDataArrayCollection : public vtkCollection
{
public:
  static vtkDataArrayCollection *New();
  vtkTypeMacro(vtkDataArrayCollection,vtkCollection);

  /**
   * Add a dataarray to the bottom of the list.
   */
  void AddItem(vtkDataArray *ds)
  {
      this->vtkCollection::AddItem(ds);
  }

  /**
   * Get the next dataarray in the list.
   */
  vtkDataArray *GetNextItem() {
    return static_cast<vtkDataArray *>(this->GetNextItemAsObject());};

  /**
   * Get the ith dataarray in the list.
   */
  vtkDataArray *GetItem(int i) {
    return static_cast<vtkDataArray *>(this->GetItemAsObject(i));};

  /**
   * Reentrant safe way to get an object in a collection. Just pass the
   * same cookie back and forth.
   */
  vtkDataArray *GetNextDataArray(vtkCollectionSimpleIterator &cookie) {
    return static_cast<vtkDataArray *>(this->GetNextItemAsObject(cookie));};

protected:
  vtkDataArrayCollection() {}
  ~vtkDataArrayCollection() override {}


private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

private:
  vtkDataArrayCollection(const vtkDataArrayCollection&) = delete;
  void operator=(const vtkDataArrayCollection&) = delete;
};


#endif
// VTK-HeaderTest-Exclude: vtkDataArrayCollection.h
