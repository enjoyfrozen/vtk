/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPURenderWindow.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPURenderWindow
 * @brief RenderWindow class that connects webGPU rendering to application windows.
 */

#ifndef vtkWebGPURenderWindow_h
#define vtkWebGPURenderWindow_h

// vtk includes
#include "vtkRenderWindow.h"
#include "vtkRenderingWebGPUModule.h" // For export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkHardwareWindow;
class vtkRenderWindowInteractor;
class vtkWebGPUWindowNode;

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPURenderWindow : public vtkRenderWindow
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPURenderWindow* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPURenderWindow, vtkRenderWindow);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * This ensures that the window is not currently rendering before calling the superclass' render.
   */
  void Render() override;

  /**
   * Starts the graph traversal
   */
  void Start() override;

  /**
   * Set the interactor for the window
   */
  void SetInteractor(vtkRenderWindowInteractor*) override;

  /**
   * Get access to the webgpu scene graph
   */
  vtkGetObjectMacro(WindowNode, vtkWebGPUWindowNode);

  ///@{
  /**
   * Set/Get the hardware window associated with the webgpu scene.
   */
  virtual vtkHardwareWindow* GetHardwareWindow();
  virtual void SetHardwareWindow(vtkHardwareWindow* win);
  ///@}

protected:
  vtkWebGPURenderWindow();
  ~vtkWebGPURenderWindow();

  // Helper members
  vtkWebGPUWindowNode* WindowNode;

private:
  vtkWebGPURenderWindow(const vtkWebGPURenderWindow&) = delete;
  void operator=(const vtkWebGPURenderWindow) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPURenderWindow_h
