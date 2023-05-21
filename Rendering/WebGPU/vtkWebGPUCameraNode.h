/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUCameraNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUCameraNode
 * @brief WebGPU camera node
 *
 */

#ifndef vtkWebGPUCameraNode_h
#define vtkWebGPUCameraNode_h

// vtk includes
#include "vtkCameraNode.h"
#include "vtkRenderingWebGPUModule.h" // for export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class vtkWebGPUCameraNode : public vtkCameraNode
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUCameraNode* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUCameraNode, vtkCameraNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

protected:
  vtkWebGPUCameraNode();
  ~vtkWebGPUCameraNode();

  // Helper members

private:
  vtkWebGPUCameraNode(const vtkWebGPUCameraNode&) = delete;
  void operator=(const vtkWebGPUCameraNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUCameraNode_h
