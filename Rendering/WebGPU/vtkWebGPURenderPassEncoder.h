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

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUEncoder.h"

// STL includes
#include <map>
#include <vector>

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUMapperNode;
class vtkWebGPUPipeline;
class vtkWebGPUTextureView;
struct WGPURenderPassDescriptor;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURenderPassEncoder : public vtkWebGPUEncoder
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
  vtkTypeMacro(vtkWebGPURenderPassEncoder, vtkWebGPUEncoder);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Get a handle to the render pass encoder
   */
  void* GetHandle() override;

  /**
   * Get access to the descriptor
   */
  WGPURenderPassDescriptor& GetDescriptor();

  ///@{
  /**
   * Begin/end the render pass.
   * Requires a valid webgpu instance.
   */
  void Begin() override;
  void Draw();
  void End() override;
  ///@}

  ///@{
  /**
   * Set/Get color texture views for the render pass
   */
  virtual void AddColorTextureView(vtkWebGPUTextureView*);
  virtual vtkWebGPUTextureView* GetColorTextureView(int idx);
  virtual std::size_t GetNumberOfColorTextureViews();
  virtual void ClearColorTextureViews();
  ///@}

  ///@{
  /**
   * Set/Get the depth texture view for the render pass
   */
  virtual void SetDepthTextureView(vtkWebGPUTextureView*);
  vtkGetObjectMacro(DepthTextureView, vtkWebGPUTextureView);
  ///@}

  /**
   * Attach the texture views to the render pass
   */
  virtual void AttachTextureViews();

  ///@{
  /**
   * Set/Get color op values
   */
  vtkSetMacro(ColorLoadOp, int);
  vtkGetMacro(ColorLoadOp, int);
  vtkSetMacro(ColorStoreOp, int);
  vtkGetMacro(ColorStoreOp, int);
  vtkSetVector4Macro(ClearColor, double);
  vtkGetVector4Macro(ClearColor, double);
  ///@}

  ///@{
  /**
   * Set/Get depth op values
   */
  vtkSetMacro(DepthLoadOp, int);
  vtkGetMacro(DepthLoadOp, int);
  vtkSetMacro(DepthStoreOp, int);
  vtkGetMacro(DepthStoreOp, int);
  vtkSetMacro(ClearDepth, double);
  vtkGetMacro(ClearDepth, double);
  ///@}

  ///@{
  /**
   * Set/Get depth op values
   */
  vtkSetMacro(StencilLoadOp, int);
  vtkGetMacro(StencilLoadOp, int);
  vtkSetMacro(StencilStoreOp, int);
  vtkGetMacro(StencilStoreOp, int);
  vtkSetMacro(ClearStencil, uint32_t);
  vtkGetMacro(ClearStencil, uint32_t);
  ///@}

  /**
   * Set the render pipeline
   */
  virtual void SetPipeline(vtkWebGPUPipeline*) override;

  /**
   * Register a pair of pipeline and drawing mapper to the encoder
   */
  virtual void RegisterPipelineMapper(vtkWebGPUPipeline*, vtkWebGPUMapperNode*);

protected:
  vtkWebGPURenderPassEncoder();
  ~vtkWebGPURenderPassEncoder();

  // Helper members
  std::vector<vtkWebGPUTextureView*> ColorTextureViews;
  vtkWebGPUTextureView* DepthTextureView = nullptr;

  int ColorLoadOp = 2;  // Load
  int ColorStoreOp = 1; // Store
  double ClearColor[4] = { 0.0, 0.0, 0.0, 0.0 };
  int DepthLoadOp = 1;  // Clear
  int DepthStoreOp = 1; // Store
  double ClearDepth = 1.0;
  int StencilLoadOp = 1;  // Clear
  int StencilStoreOp = 1; // Store
  uint32_t ClearStencil = 0;

  /**
   * Map of pipelines to mappers that draw in the current pass
   * This map is cleared at the beginning of each pass i.e. in Begin().
   */
  std::map<vtkWebGPUPipeline*, std::vector<vtkWebGPUMapperNode*>> DrawingMappers;

private:
  class vtkInternal;
  vtkInternal* Internal;

  vtkWebGPURenderPassEncoder(const vtkWebGPURenderPassEncoder&) = delete;
  void operator=(const vtkWebGPURenderPassEncoder) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPURenderPassEncoder_h
