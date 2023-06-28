/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUUniformBuffer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUUniformBuffer
 * @brief A bindable object that represents a webgpu uniform buffer
 *
 */

#ifndef vtkWebGPUUniformBuffer_h
#define vtkWebGPUUniformBuffer_h

// vtk includes
#include "vtkWebGPUBuffer.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class vtkWebGPUUniformBuffer : public vtkWebGPUBuffer
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUUniformBuffer* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUUniformBuffer, vtkWebGPUBuffer);
  void PrintSelf(ostream &os, vtkIndent indent) override;
  ///@}

  /**
   * Override to add the binding layout
   */
  void* GetBindGroupLayoutEntry() override;

protected:
  vtkWebGPUUniformBuffer();
  ~vtkWebGPUUniformBuffer();

  // Helper members

private:
  // Internals
  class vtkInternal;
  vtkInternal* Internal = nullptr;

  vtkWebGPUUniformBuffer(const vtkWebGPUUniformBuffer&) = delete;
  void operator=(const vtkWebGPUUniformBuffer) = delete;
};

VTK_ABI_NAMESPACE_END
#endif //vtkWebGPUUniformBuffer_h
