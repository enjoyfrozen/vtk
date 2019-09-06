/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOverrideInformationCollection
 * @brief   maintain a list of override information objects
 *
 * vtkOverrideInformationCollection is an object that creates and manipulates
 * lists of objects of type vtkOverrideInformation.
 * @sa
 * vtkCollection
*/

#ifndef vtkOverrideInformationCollection_h
#define vtkOverrideInformationCollection_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkCollection.h"

#include "vtkOverrideInformation.h" // Needed for inline methods

class VTKCOMMONCORE_EXPORT vtkOverrideInformationCollection : public vtkCollection
{
public:
  vtkTypeMacro(vtkOverrideInformationCollection,vtkCollection);
  static vtkOverrideInformationCollection *New();

  /**
   * Add a OverrideInformation to the list.
   */
  void AddItem(vtkOverrideInformation *);

  /**
   * Get the next OverrideInformation in the list.
   */
  vtkOverrideInformation *GetNextItem();

  /**
   * Reentrant safe way to get an object in a collection. Just pass the
   * same cookie back and forth.
   */
  vtkOverrideInformation *GetNextOverrideInformation(
    vtkCollectionSimpleIterator &cookie) {
    return static_cast<vtkOverrideInformation *>(
      this->GetNextItemAsObject(cookie));};

protected:
  vtkOverrideInformationCollection() {}
  ~vtkOverrideInformationCollection() override {}


private:
  // hide the standard AddItem from the user and the compiler.
  void AddItem(vtkObject *o) { this->vtkCollection::AddItem(o); };

private:
  vtkOverrideInformationCollection(const vtkOverrideInformationCollection&) = delete;
  void operator=(const vtkOverrideInformationCollection&) = delete;
};

inline void vtkOverrideInformationCollection::AddItem(vtkOverrideInformation *f)
{
  this->vtkCollection::AddItem(f);
}

inline vtkOverrideInformation *vtkOverrideInformationCollection::GetNextItem()
{
  return static_cast<vtkOverrideInformation *>(this->GetNextItemAsObject());
}

#endif
// VTK-HeaderTest-Exclude: vtkOverrideInformationCollection.h
