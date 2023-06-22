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
#include "vtkWebGPUViewNode.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkWebGPURenderPassEncoder;

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
  virtual void PrepareAndDraw(vtkWebGPURenderPassEncoder*);

  /**
   * Do everything necessary for this mapper node to be ready to draw but do not bind or call the
   * actual draw commands.
   */
  virtual void PrepareToDraw(vtkWebGPURenderPassEncoder*);

  /**
   * Bind and draw
   * When this function is called, it is required that the command encoder and pipeline be
   * initialized and bound.
   */
  virtual void Draw(vtkWebGPURenderPassEncoder*);

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

private:
  vtkWebGPUMapperNode(const vtkWebGPUMapperNode&) = delete;
  void operator=(const vtkWebGPUMapperNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUMapperNode_h
