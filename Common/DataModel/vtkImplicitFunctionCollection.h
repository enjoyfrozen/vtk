/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImplicitFunctionCollection
 * @brief   maintain a list of implicit functions
 *
 * vtkImplicitFunctionCollection is an object that creates and manipulates
 * lists of objects of type vtkImplicitFunction.
 * @sa
 * vtkCollection vtkPlaneCollection
*/

#ifndef vtkImplicitFunctionCollection_h
#define vtkImplicitFunctionCollection_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkCollection.h"

#include "vtkImplicitFunction.h" // Needed for inline methods

class VTKCOMMONDATAMODEL_EXPORT vtkImplicitFunctionCollection : public vtkCollection
{
public:
  vtkTypeMacro(vtkImplicitFunctionCollection,vtkCollection);
  static vtkImplicitFunctionCollection *New();

  /**
   * Add an implicit function to the list.
   */
  void AddItem(vtkImplicitFunction *);

  /**
   * Get the next implicit function in the list.
   */
  vtkImplicitFunction *GetNextItem();

  //@{
  /**
   * Reentrant safe way to get an object in a collection. Just pass the
   * same cookie back and forth.
   */
  vtkImplicitFunction *GetNextImplicitFunction(
    vtkCollectionSimpleIterator &cookie)
  {
      return static_cast<vtkImplicitFunction *>(
        this->GetNextItemAsObject(cookie));
  };
  //@}

protected:
  vtkImplicitFunctionCollection() {}
  ~vtkImplicitFunctionCollection() override {}


private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

private:
  vtkImplicitFunctionCollection(const vtkImplicitFunctionCollection&) = delete;
  void operator=(const vtkImplicitFunctionCollection&) = delete;
};

inline void vtkImplicitFunctionCollection::AddItem(vtkImplicitFunction *f)
{
  this->vtkCollection::AddItem(f);
}

inline vtkImplicitFunction *vtkImplicitFunctionCollection::GetNextItem()
{
 return static_cast<vtkImplicitFunction *>(this->GetNextItemAsObject());
}

#endif
// VTK-HeaderTest-Exclude: vtkImplicitFunctionCollection.h
