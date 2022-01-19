/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWeakReference.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

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
#include "vtkDeprecation.h"      // For VTK_DEPRECATED_IN_9_2_0
#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include "vtkWeakPtr.h"

class VTKCOMMONCORE_EXPORT vtkWeakReference : public vtkObject
{
public:
  vtkTypeMacro(vtkWeakReference, vtkObject);
  static vtkWeakReference* New();
  vtkWeakReference();
  ~vtkWeakReference() override;

  /**
   * Set the vtkObject to maintain a weak reference to.
   */
  void Set(vtkObject* object);

  /**
   * Get the vtkObject pointer or nullptr if the object has been collected.
   */
  VTK_DEPRECATED_IN_9_2_0(
    "Use GetOwned() which ensures the caller has a reference that lives long enough")
  vtkObject* Get();

  /**
   * Get the vtkObject pointer or nullptr if the object has been collected.
   *
   * The returned object has an increased reference count to make sure it is
   * useful for as long as the caller needs it.
   */
  vtkSmartPointer<vtkObject> GetOwned() const;

private:
  vtkWeakPtr<vtkObject> Object;
};

#endif

// VTK-HeaderTest-Exclude: vtkWeakReference.h
