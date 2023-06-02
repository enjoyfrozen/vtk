/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSDL2HardwareWindow.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkSDL2HardwareWindow
 * @brief represents a window for the SDL2 emscripten backend
 *
 */

#ifndef vtkSDL2HardwareWindow_h
#define vtkSDL2HardwareWindow_h

// vtk includes
#include "vtkHardwareWindow.h"
#include "vtkRenderingUIModule.h" // For export macro
// Ignore reserved-identifier warnings from
// 1. SDL2/SDL_stdinc.h: warning: identifier '_SDL_size_mul_overflow_builtin'
// 2. SDL2/SDL_stdinc.h: warning: identifier '_SDL_size_add_overflow_builtin'
// 3. SDL2/SDL_audio.h: warning: identifier '_SDL_AudioStream'
// 4. SDL2/SDL_joystick.h: warning: identifier '_SDL_Joystick'
// 5. SDL2/SDL_sensor.h: warning: identifier '_SDL_Sensor'
// 6. SDL2/SDL_gamecontroller.h: warning: identifier '_SDL_GameController'
// 7. SDL2/SDL_haptic.h: warning: identifier '_SDL_Haptic'
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#include "SDL.h" // for ivar
#ifdef __clang__
#pragma clang diagnostic pop
#endif

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGUI_EXPORT vtkSDL2HardwareWindow : public vtkHardwareWindow
{
public:
  /**
   * Instantiate the class.
   */
  static vtkSDL2HardwareWindow* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkSDL2HardwareWindow, vtkHardwareWindow);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  void Create() override;
  void Destroy() override;

  ///@{
  /**
   * Hide or Show the mouse cursor, it is nice to be able to hide the
   * default cursor if you want VTK to display a 3D cursor instead.
   */
  void HideCursor() override;
  void ShowCursor() override;
  ///@}

  /**
   * Set the name of the window. This appears at the top of the window
   * normally.
   */
  void SetWindowName(const char*) override;

  /**
   * Get this RenderWindow's X window id.
   */
  SDL_Window* GetWindowId();

  ///@{
  /**
   * Set/Get the position of the window.
   */
  void SetPosition(int, int) override;
  void SetPosition(int a[2]) override { this->SetPosition(a[0], a[1]); }
  int* GetPosition() VTK_SIZEHINT(2) override;
  ///@}

  ///@{
  /**
   * Set/Get the size of the window in pixels.
   */
  void SetSize(int, int) override;
  using vtkHardwareWindow::SetSize;
  int* GetSize() VTK_SIZEHINT(2) override;
  ///@}

protected:
  vtkSDL2HardwareWindow();
  ~vtkSDL2HardwareWindow();

  // Helper members
  SDL_Window* WindowId = nullptr;
  static const std::string DEFAULT_BASE_WINDOW_NAME;

private:
  vtkSDL2HardwareWindow(const vtkSDL2HardwareWindow&) = delete;
  void operator=(const vtkSDL2HardwareWindow) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkSDL2HardwareWindow_h
