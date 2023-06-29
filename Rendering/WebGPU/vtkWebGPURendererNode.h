/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURendererNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPURendererNode
 * @brief WebGPU renderer view node
 *
 */

#ifndef vtkWebGPURendererNode_h
#define vtkWebGPURendererNode_h

// VTK includes
#include "vtkRendererNode.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkCamera;
class vtkWebGPUBindGroup;
class vtkWebGPURenderPassEncoder;
class vtkWebGPUStorageBuffer;
class vtkWebGPUUniformBuffer;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURendererNode : public vtkRendererNode
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPURendererNode* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPURendererNode, vtkRendererNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  ///@{
  /**
   * Set/Get the render pass encoder
   */
  virtual void SetRenderEncoder(vtkWebGPURenderPassEncoder*);
  vtkGetObjectMacro(RenderEncoder, vtkWebGPURenderPassEncoder);
  ///@}

  /**
   * Build pass
   */
  void Build(bool prepass) override;

  /**
   * Opaque pass
   */
  virtual void OpaquePass(bool prepass);

  /**
   * Setup the viewport and scissor rect for a render pass
   */
  virtual void ScissorAndViewport(vtkWebGPURenderPassEncoder*);

  /**
   * Clear pass
   */
  virtual void Clear();

  /**
   * WebGPU follows the DirecX/Metal coordinate system where the origin is the top left corner.
   * Use this method to get the webgpu origin from the VTK default origin (bottom-left corner).
   */
  virtual void GetYInvertedTiledSizeAndOrigin(
    int* width, int* height, int* topLeftX, int* topLeftY);

  /**
   * Get access to the bind group
   */
  vtkGetObjectMacro(BindGroup, vtkWebGPUBindGroup);

  /**
   * Activate this renderer's bind group
   */
  virtual void ActivateBindGroup();

protected:
  vtkWebGPURendererNode();
  ~vtkWebGPURendererNode();

  // Helper members
  vtkWebGPURenderPassEncoder* RenderEncoder = nullptr;
  vtkCamera* Camera = nullptr;
  double StabilizedCenter[3] = { 0, 0, 0 };
  double RecenterThreshold = 20.0;
  vtkWebGPUBindGroup* BindGroup = nullptr;
  vtkWebGPUUniformBuffer* UBO = nullptr;
  vtkWebGPUStorageBuffer* SSBO = nullptr;

  /**
   * Update lights for the renderer
   */
  virtual int UpdateLights();

  /**
   * This method is designed to help with floating point
   * issues when rendering datasets that push the limits of
   * resolutions on float.
   *
   * One of the most common cases is when the dataset is located far
   * away from the origin relative to the clipping range we are looking
   * at. For that case we want to perform the floating point sensitive
   * multiplications on the CPU in double. To this end we want the
   * vertex rendering ops to look something like
   *
   * Compute shifted points and load those into the VBO
   * pointCoordsSC = WorldToStabilizedMatrix * pointCoords;
   *
   * In the vertex shader do the following
   * positionVC = StabilizedToDeviceMatrix * ModelToStabilizedMatrix*vertexIn;
   *
   * We use two matrices because it is expensive to change the
   * WorldToStabilized matrix as we have to reupload all pointCoords
   * So that matrix (MCSCMatrix) is fairly static, the Stabilized to
   * Device matrix is the one that gets updated every time the camera
   * changes.
   *
   * The basic idea is that we should translate the data so that
   * when the center of the view frustum moves a lot
   * we recenter it. The center of the view frustum is roughly
   * camPos + dirOfProj*(far + near)*0.5
   */
  void UpdateStabilizedMatrix();

private:
  vtkWebGPURendererNode(const vtkWebGPURendererNode&) = delete;
  void operator=(const vtkWebGPURendererNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPURendererNode_h
