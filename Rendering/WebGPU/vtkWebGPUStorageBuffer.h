/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUStorageBuffer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUStorageBuffer
 * @brief A bindable object that represents a webgpu storage buffer
 *
 */

#ifndef vtkWebGPUStorageBuffer_h
#define vtkWebGPUStorageBuffer_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUBuffer.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class vtkWebGPUStorageBuffer : public vtkWebGPUBuffer
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUStorageBuffer* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUStorageBuffer, vtkWebGPUBuffer);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Override to add the binding layout
   */
  void* GetBindGroupLayoutEntry() override;

protected:
  vtkWebGPUStorageBuffer();
  ~vtkWebGPUStorageBuffer();

  // Helper members

private:
  // Internals
  class vtkInternal;
  vtkInternal* Internal = nullptr;

  vtkWebGPUStorageBuffer(const vtkWebGPUStorageBuffer&) = delete;
  void operator=(const vtkWebGPUStorageBuffer) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUStorageBuffer_h
