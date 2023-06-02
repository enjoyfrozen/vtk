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
class vtkRenderPassCollection;
class vtkRenderWindowInteractor;
class vtkSceneGraphRenderPass;

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
  vtkGetObjectMacro(RenderPasses, vtkRenderPassCollection);
  ///@}

  /**
   * Build pass
   * Initialize webgpu
   * Setup hardware window
   */
  void Build(bool prepass) override;

protected:
  vtkWebGPUWindowNode();
  ~vtkWebGPUWindowNode();

  // Helper members
  vtkRenderWindowInteractor* Interactor = nullptr;
  vtkHardwareWindow* HardwareWindow = nullptr;
  vtkRenderPassCollection* RenderPasses = nullptr;

  std::atomic<int> NextSize[2];
  int Size[2];

  // API to check whether we are initialized
  vtkTypeBool IsInitialized();

private:
  // Internals
  class vtkInternal;
  vtkInternal* Internal = nullptr;

  vtkWebGPUWindowNode(const vtkWebGPUWindowNode&) = delete;
  void operator=(const vtkWebGPUWindowNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUWindowNode_h
