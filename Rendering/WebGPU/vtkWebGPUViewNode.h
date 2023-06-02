/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUViewNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUViewNode
 * @brief Abstract base for webgpu view nodes
 */

#ifndef vtkWebGPUViewNode_h
#define vtkWebGPUViewNode_h

// vtk includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkViewNode.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUViewNode : public vtkViewNode
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUViewNode* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUViewNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  enum WebGPU_Operations
  {
    query = vtkViewNode::invalidate + 1,
    opaquePass
  };

  /**
   * Query pass
   */
  virtual void Query(bool /* prepass */) {}

  /**
   * Opaque pass
   */
  virtual void OpaquePass(bool /* prepass */) {}

protected:
  vtkWebGPUViewNode();
  ~vtkWebGPUViewNode();

  // Helper members
  void Apply(int operation, bool prepass) override;

private:
  vtkWebGPUViewNode(const vtkWebGPUViewNode&) = delete;
  void operator=(const vtkWebGPUViewNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUViewNode_h
