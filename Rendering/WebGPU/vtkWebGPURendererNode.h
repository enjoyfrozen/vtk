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

protected:
  vtkWebGPURendererNode();
  ~vtkWebGPURendererNode();

  // Helper members

private:
  vtkWebGPURendererNode(const vtkWebGPURendererNode&) = delete;
  void operator=(const vtkWebGPURendererNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPURendererNode_h
