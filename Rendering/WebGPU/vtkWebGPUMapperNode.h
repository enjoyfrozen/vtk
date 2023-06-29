/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUMapperNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUMapperNode
 * @brief Simple mapper node for webgpu
 */

#ifndef vtkWebGPUMapperNode_h
#define vtkWebGPUMapperNode_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWebGPUType.h"            // for topology
#include "vtkWebGPUViewNode.h"

// STL includes
#include <set>    // for set
#include <vector> // for vector

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPUBindGroup;
class vtkWebGPUBindableObject;
class vtkWebGPUPipeline;
class vtkWebGPURenderPassEncoder;
class vtkWebGPURendererNode;
class vtkWebGPUStorageBuffer;
class vtkWebGPUTextureView;
class vtkWebGPUUniformBuffer;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUMapperNode : public vtkWebGPUViewNode
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUMapperNode* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUMapperNode, vtkWebGPUViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Prepare the node and draw
   */
  virtual void PrepareAndDraw();

  /**
   * Do everything necessary for this mapper node to be ready to draw but do not bind or call the
   * actual draw commands.
   */
  virtual void PrepareToDraw();

  /**
   * Bind and draw
   * When this function is called, it is required that the command encoder and pipeline be
   * initialized and bound.
   */
  virtual void Draw();

  ///@{
  /**
   * Set/Get the render pass encoder
   */
  virtual void SetRenderPassEncoder(vtkWebGPURenderPassEncoder*);
  vtkGetObjectMacro(RenderPassEncoder, vtkWebGPURenderPassEncoder);
  ///@}

  ///@{
  /**
   * Set/Get the render pipeline
   */
  virtual void SetPipeline(vtkWebGPUPipeline*);
  vtkGetObjectMacro(Pipeline, vtkWebGPUPipeline);

  ///@{
  /*
   * Set/Get bindables for the mapper
   */
  virtual void SetBindables(std::vector<vtkWebGPUBindableObject*> b);
  virtual std::vector<vtkWebGPUBindableObject*> GetBindables();
  ///@}

  ///@{
  /**
   * Set/Get the UBO
   */
  virtual void SetUBO(vtkWebGPUUniformBuffer* ubo);
  vtkGetObjectMacro(UBO, vtkWebGPUUniformBuffer);
  ///@}

  ///@{
  /**
   * Set/Get the SSBO
   */
  virtual void SetSSBO(vtkWebGPUStorageBuffer* ubo);
  vtkGetObjectMacro(SSBO, vtkWebGPUStorageBuffer);
  ///@}

  ///@{
  /**
   * Set/Get the renderer
   */
  virtual void SetRenderer(vtkWebGPURendererNode*);
  vtkGetObjectMacro(Renderer, vtkWebGPURendererNode);
  ///@}

  ///@{
  /**
   * To ensure that there are a unique set of pipelines, we map them via a unique string called the
   * pipeline hash. The pipelines map is cached by the device
   */
  virtual void ComputePipelineHash();
  vtkSetStdStringFromCharMacro(PipelineHash);
  vtkGetCharFromStdStringMacro(PipelineHash);
  ///@}

  /**
   * Generate shader descriptions for the pipeline
   */
  virtual void GenerateShaderDescriptions(vtkWebGPUPipeline*);

  ///@(
  /**
   * Set/Get the primitive topology
   */
  vtkSetClampMacro(
    PrimitiveTopology, int, vtkWebGPUType::PointList, vtkWebGPUType::NumberOfPrimitiveTopologies);
  vtkGetMacro(PrimitiveTopology, int);
  ///@}

  /**
   * Get access to the mapper's bind group
   */
  vtkGetObjectMacro(BindGroup, vtkWebGPUBindGroup);

protected:
  vtkWebGPUMapperNode();
  ~vtkWebGPUMapperNode();

  // Helper members
  /**
   * Update the input
   */
  virtual void UpdateInput();

  /**
   * Create and update buffers
   */
  virtual void UpdateBuffers();

  /**
   * Update bindings and bind groups/layouts
   */
  virtual void UpdateBindings();

  /**
   * Update the pipeline
   */
  virtual void UpdatePipeline();

  vtkWebGPURenderPassEncoder* RenderPassEncoder = nullptr;
  vtkWebGPUPipeline* Pipeline = nullptr;
  vtkWebGPUBindGroup* BindGroup;
  vtkWebGPUUniformBuffer* UBO = nullptr;
  vtkWebGPURendererNode* Renderer = nullptr;
  vtkWebGPUStorageBuffer* SSBO = nullptr;

  std::string PipelineHash;

  std::vector<vtkWebGPUBindableObject*> Bindables;
  std::set<vtkWebGPUTextureView*> TextureViews;

  int PrimitiveTopology = vtkWebGPUType::TriangleList;

private:
  vtkWebGPUMapperNode(const vtkWebGPUMapperNode&) = delete;
  void operator=(const vtkWebGPUMapperNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUMapperNode_h
