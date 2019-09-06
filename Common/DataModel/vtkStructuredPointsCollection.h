/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkStructuredPointsCollection
 * @brief   maintain a list of structured points data objects
 *
 * vtkStructuredPointsCollection is an object that creates and manipulates
 * ordered lists of structured points datasets. See also vtkCollection and
 * subclasses.
*/

#ifndef vtkStructuredPointsCollection_h
#define vtkStructuredPointsCollection_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkCollection.h"
#include "vtkStructuredPoints.h" // Needed for static cast

class VTKCOMMONDATAMODEL_EXPORT vtkStructuredPointsCollection : public vtkCollection
{
public:
  static vtkStructuredPointsCollection *New();
  vtkTypeMacro(vtkStructuredPointsCollection,vtkCollection);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Add a pointer to a vtkStructuredPoints to the bottom of the list.
   */
  void AddItem(vtkStructuredPoints *ds)
  {
      this->vtkCollection::AddItem(ds);
  }

  /**
   * Get the next item in the collection. nullptr is returned if the collection
   * is exhausted.
   */
  vtkStructuredPoints *GetNextItem() {
    return static_cast<vtkStructuredPoints *>(this->GetNextItemAsObject());};

  /**
   * Reentrant safe way to get an object in a collection. Just pass the
   * same cookie back and forth.
   */
  vtkStructuredPoints *GetNextStructuredPoints(
    vtkCollectionSimpleIterator &cookie) {
    return static_cast<vtkStructuredPoints *>(
      this->GetNextItemAsObject(cookie));};

protected:
  vtkStructuredPointsCollection() {}
  ~vtkStructuredPointsCollection() override {}



private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

private:
  vtkStructuredPointsCollection(const vtkStructuredPointsCollection&) = delete;
  void operator=(const vtkStructuredPointsCollection&) = delete;
};


#endif
