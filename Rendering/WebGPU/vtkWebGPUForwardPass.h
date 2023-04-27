/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUForwardPass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUForwardPass
 * @brief A forward rendering pipeline for webgpu
 *
 */

#ifndef vtkWebGPUForwardPass_h
#define vtkWebGPUForwardPass_h

// VTK includes
#include "vtkRenderPass.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUForwardPass : public vtkRenderPass
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUForwardPass* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUForwardPass, vtkRenderPass);
  void PrintSelf(ostream &os, vtkIndent indent) override;
  ///@}

protected:
  vtkWebGPUForwardPass();
  ~vtkWebGPUForwardPass();

  // Helper members

private:
  vtkWebGPUForwardPass(const vtkWebGPUForwardPass&) = delete;
  void operator=(const vtkWebGPUForwardPass) = delete;
};

VTK_ABI_NAMESPACE_END
#endif //vtkWebGPUForwardPass_h
