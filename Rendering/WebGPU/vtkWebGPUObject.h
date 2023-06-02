/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUObject.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUObject
 * @brief An object class for the webgpu backend
 *
 */

#ifndef vtkWebGPUObject_h
#define vtkWebGPUObject_h

// VTK includes
#include "vtkObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUObject : public vtkObject
{
public:
  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUObject, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Set/Get the label
   */
  vtkSetStringMacro(Label);
  vtkGetStringMacro(Label);
  ///@}

  /**
   * Returns the webgpu object.
   * Must be implemented by each sub-class.
   */
  virtual void* GetHandle() = 0;

protected:
  vtkWebGPUObject();
  ~vtkWebGPUObject();

  // Helper members
  char* Label = nullptr;

private:
  vtkWebGPUObject(const vtkWebGPUObject&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUObject_h
