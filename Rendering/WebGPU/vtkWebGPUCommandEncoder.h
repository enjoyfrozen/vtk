/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUCommandEncoder.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUCommandEncoder
 * @brief Command encoder for the webgpu backend
 *
 */

#ifndef vtkWebGPUCommandEncoder_h
#define vtkWebGPUCommandEncoder_h

// vtk includes
#include "vtkWebGPUObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
struct WGPUCommandEncoderDescriptor;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUCommandEncoder : public vtkWebGPUObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUCommandEncoder* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUCommandEncoder, vtkWebGPUObject);
  void PrintSelf(ostream &os, vtkIndent indent) override;
  ///@}

  /**
   * Get a handle to the render pass encoder
   */
  void* GetHandle() override;

  /**
   * Get access to the descriptor
   */
  WGPUCommandEncoderDescriptor& GetDescriptor();

  ///@{
  /**
   * Create/destroy the command encoder
   */
  virtual void Create();
  virtual void Destroy();
  ///@}

protected:
  vtkWebGPUCommandEncoder();
  ~vtkWebGPUCommandEncoder();

  // Helper members

private:
  class vtkInternal;
  vtkInternal* Internal;

  vtkWebGPUCommandEncoder(const vtkWebGPUCommandEncoder&) = delete;
  void operator=(const vtkWebGPUCommandEncoder) = delete;
};

VTK_ABI_NAMESPACE_END
#endif //vtkWebGPUCommandEncoder_h
