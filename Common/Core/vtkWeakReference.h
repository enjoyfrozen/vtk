/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkWeakReference
 * @brief   Utility class to hold a weak reference to a vtkObject.
 *
 * Simple Set(...)/Get(...) interface. Used in numpy support to provide a
 * reference to a vtkObject without preventing it from being collected.
 */

#ifndef vtkWeakReference_h
#define vtkWeakReference_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkObject.h"
#include "vtkWeakPointer.h"

class VTKCOMMONCORE_EXPORT vtkWeakReference : public vtkObject
{
public:
  vtkTypeMacro(vtkWeakReference, vtkObject);
  static vtkWeakReference *New();
  vtkWeakReference();
  ~vtkWeakReference() override;

  /**
   * Set the vtkObject to maintain a weak reference to.
   */
  void Set(vtkObject *object);

  /**
   * Get the vtkObject pointer or nullptr if the object has been collected.
   */
  vtkObject* Get();

private:
  vtkWeakPointer<vtkObject> Object;
};

#endif

// VTK-HeaderTest-Exclude: vtkWeakReference.h
