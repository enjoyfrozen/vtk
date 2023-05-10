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
class vtkHardwareWindow;
class vtkRenderWindowInteractor;
class vtkWebGPUInstance;
class vtkRenderPassCollection;

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

  ///@{
  /**
   * Set/Get the webgpu instance
   */
  virtual void SetInstance(vtkWebGPUInstance*);
  vtkGetObjectMacro(Instance, vtkWebGPUInstance);
  ///@}

  /**
   * Override the explicit setting of the renderable on this node to ensure that it is a webgpu
   * render window
   */
  void SetRenderable(vtkObject*) override;

  ///@{
  /**
   * Set/Get the interactor for the window
   */
  virtual void SetInteractor(vtkRenderWindowInteractor* iren);
  vtkGetObjectMacro(Interactor, vtkRenderWindowInteractor);
  ///@}

  ///@{
  /**
   * Set/Get the hardware window for this view node
   */
  virtual void SetHardwareWindow(vtkHardwareWindow* w);
  vtkGetObjectMacro(HardwareWindow, vtkHardwareWindow);
  ///@}

  /**
   * Traverse
   */
  void TraverseAllPasses() override;

  ///@{
  /**
   * Scene graph render pass API
   */
  virtual void AddRenderPass(vtkSceneGraphRenderPass* pass);

protected:
  vtkWebGPUWindowNode();
  ~vtkWebGPUWindowNode();

  // Helper members
  vtkWebGPUInstance* Instance = nullptr;
  vtkRenderWindowInteractor* Interactor = nullptr;
  vtkHardwareWindow* HardwareWindow = nullptr;
  vtkRenderPassCollection* RenderPasses = nullptr;

private:
  vtkWebGPUWindowNode(const vtkWebGPUWindowNode&) = delete;
  void operator=(const vtkWebGPUWindowNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUWindowNode_h
