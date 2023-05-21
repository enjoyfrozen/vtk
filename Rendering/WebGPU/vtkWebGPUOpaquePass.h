/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUOpaquePass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*=============================================================================
Copyright and License information
=============================================================================*/
/**
 * @class vtkWebGPUOpaquePass
 * @brief Forward rendering pipeline for webgpu that mixes in opaque geometry and volumes.
 */

#ifndef vtkWebGPUOpaquePass_h
#define vtkWebGPUOpaquePass_h

// vtk includes
#include "vtkSceneGraphRenderPass.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPURenderPassEncoder;
class vtkWebGPUTexture;
class vtkWebGPUTextureView;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUOpaquePass : public vtkSceneGraphRenderPass
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUOpaquePass* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUOpaquePass, vtkSceneGraphRenderPass);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Override to traverse the renderer node and render opaque geometry and volumes
   */
  void Traverse(vtkViewNode* vn, vtkRenderPass* parent = nullptr) override;

protected:
  vtkWebGPUOpaquePass();
  ~vtkWebGPUOpaquePass();

  // Helper members
  vtkWebGPURenderPassEncoder* RenderEncoder = nullptr;
  vtkWebGPUTexture* ColorTexture = nullptr;
  vtkWebGPUTextureView* ColorTextureView = nullptr;
  vtkWebGPUTexture* DepthTexture = nullptr;
  vtkWebGPUTextureView* DepthTextureView = nullptr;

private:
  vtkWebGPUOpaquePass(const vtkWebGPUOpaquePass&) = delete;
  void operator=(const vtkWebGPUOpaquePass) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUOpaquePass_h
