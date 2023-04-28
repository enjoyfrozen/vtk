/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUWindowNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUWindowNode
 * @brief Translates vtkRenderWindow to webgpu
 *
 */

#ifndef vtkWebGPUWindowNode_h
#define vtkWebGPUWindowNode_h

// VTK includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkWindowNode.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUWindowNode : public vtkWindowNode
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUWindowNode* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUWindowNode, vtkWindowNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

protected:
  vtkWebGPUWindowNode();
  ~vtkWebGPUWindowNode();

  // Helper members

private:
  vtkWebGPUWindowNode(const vtkWebGPUWindowNode&) = delete;
  void operator=(const vtkWebGPUWindowNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUWindowNode_h
