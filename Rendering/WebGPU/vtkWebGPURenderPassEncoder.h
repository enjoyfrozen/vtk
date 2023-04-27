/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURenderPassEncoder.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPURenderPassEncoder
 * @brief Render pass encoder for webgpu
 *
 */

#ifndef vtkWebGPURenderPassEncoder_h
#define vtkWebGPURenderPassEncoder_h

// VTK includes
#include "vtkObject.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURenderPassEncoder : public vtkObject
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPURenderPassEncoder* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPURenderPassEncoder, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

protected:
  vtkWebGPURenderPassEncoder();
  ~vtkWebGPURenderPassEncoder();

  // Helper members

private:
  vtkWebGPURenderPassEncoder(const vtkWebGPURenderPassEncoder&) = delete;
  void operator=(const vtkWebGPURenderPassEncoder) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPURenderPassEncoder_h
