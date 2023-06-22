/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUBindableObject.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUBindableObject
 * @brief A bindable object for the webgpu backend
 */

#ifndef vtkWebGPUBindableObject_h
#define vtkWebGPUBindableObject_h

// vtk includes
#include "vtkWebGPUObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUBindableObject : public vtkWebGPUObject
{
public:
  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUBindableObject, vtkWebGPUObject);
  void PrintSelf(ostream &os, vtkIndent indent) override;
  ///@}

  /**
   * Get the bind group MTime
   */
  virtual vtkMTimeType GetBindGroupTime() const;

  /**
   * Get the bind group entry
   * Must be implemented by sub-classes.
   */
  virtual void* GetBindGroupEntry() = 0;

  /**
   * Get the bind group layout entry
   * Must be implemented by sub-classes.
   */
  virtual void* GetBindGroupLayoutEntry() = 0;

protected:
  vtkWebGPUBindableObject();
  ~vtkWebGPUBindableObject();

  // Helper members
  vtkTimeStamp BindGroupTime;

private:
  vtkWebGPUBindableObject(const vtkWebGPUBindableObject&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif //vtkWebGPUBindableObject_h
