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
#include "vtkWebGPUObject.h"

// STL includes
#include <vector>

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUTextureView;
struct WGPURenderPassDescriptor;
struct WGPUCommandEncoderImpl;
typedef WGPUCommandEncoderImpl* WGPUCommandEncoder;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURenderPassEncoder : public vtkWebGPUObject
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
  vtkTypeMacro(vtkWebGPURenderPassEncoder, vtkWebGPUObject);
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

  void Begin(WGPUCommandEncoder enc);
  void End();

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

protected:
  vtkWebGPURenderPassEncoder();
  ~vtkWebGPURenderPassEncoder();

  // Helper members
  std::vector<vtkWebGPUTextureView*> ColorTextureViews;
  vtkWebGPUTextureView* DepthTextureView = nullptr;

  int ColorLoadOp = 2; // Load
  int ColorStoreOp = 1; // Store
  double ClearColor[4] = {0.0, 0.0, 0.0, 0.0};
  int DepthLoadOp = 1; // Clear
  int DepthStoreOp = 1; // Store
  double ClearDepth = 1.0;
  int StencilLoadOp = 1; // Clear
  int StencilStoreOp = 1; // Store
  uint32_t ClearStencil = 0;

private:
  class vtkInternal;
  vtkInternal* Internal;

  vtkWebGPURenderPassEncoder(const vtkWebGPURenderPassEncoder&) = delete;
  void operator=(const vtkWebGPURenderPassEncoder) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPURenderPassEncoder_h
