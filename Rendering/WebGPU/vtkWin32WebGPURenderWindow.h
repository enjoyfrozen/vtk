/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWin32WebGPURenderWindow.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWin32WebGPURenderWindow
 * @brief WebGPU rendering window
 *
 * vtkWin32WebGPURenderWindow is a concrete implementation of the abstract class vtkRenderWindow.
 * vtkWin32WebGPURenderWindow interfaces to the standard WebGPU graphics library in the Windows/NT
 * environment.
 */

#ifndef vtkWin32WebGPURenderWindow_h
#define vtkWin32WebGPURenderWindow_h

#include "vtkWebGPURenderWindow.h"
#include "vtkRenderingWebGPUModule.h" // For export macro
#include "vtkWindows.h" // For windows API

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWin32WebGPURenderWindow : public vtkWebGPURenderWindow
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWin32WebGPURenderWindow* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWin32WebGPURenderWindow, vtkWebGPURenderWindow);
  void PrintSelf(ostream &os, vtkIndent indent) override;
  ///@}

  /**
   * End the rendering process and display the image.
   */
  void Frame() override;

  /**
   * Initialize the window for rendering.
   */
  virtual void WindowInitialize();

  /**
   * Initialize the rendering window.  This will setup all system-specific
   * resources.  This method and Finalize() must be symmetric and it
   * should be possible to call them multiple times, even changing WindowId
   * in-between.  This is what WindowRemap does.
   */
  void Initialize() override;

  /**
   * "Deinitialize" the rendering window.  This will shutdown all system-specific
   * resources.  After having called this, it should be possible to destroy
   * a window that was used for a SetWindowId() call without any ill effects.
   */
  void Finalize() override;

  /**
   * Change the window to fill the entire screen.
   */
  void SetFullScreen(vtkTypeBool) override;

  /**
   * Resize the window.
   */
  void WindowRemap() override;

  // Call X funcs to map unmap
  void SetShowWindow(bool val) override;

  /**
   * Set the preferred window size to full screen.
   */
  virtual void PrefFullScreen();

  /**
   * Set the size (width and height in pixels) of the rendering window.
   * If this is a toplevel window with borders, then the request for a
   * new size is redirected to the window manager. If the window manager
   * chooses a different size for the window, the size it chooses will
   * take effect at the next render, otherwise the size change will take
   * effect immediately. In the rare case that the window manager does
   * does not respond at all (buggy/frozen window manager), the SetSize()
   * method will wait for the response for two seconds before returning.
   *
   * If the size has changed, a vtkCommand::WindowResizeEvent will fire.
   */
  void SetSize(int width, int height) override;
  void SetSize(int a[2]) override { this->SetSize(a[0], a[1]); }

  /**
   * Get the size (width and height) of the rendering window in
   * screen coordinates (in pixels).
   */
  int* GetSize() VTK_SIZEHINT(2) override;

  ///@{
  /**
   * Set the position (x and y) of the rendering window in
   * screen coordinates (in pixels). This resizes the operating
   * system's view/window and redraws it.
   */
  void SetPosition(int x, int y) override;
  void SetPosition(int a[2]) override { this->SetPosition(a[0], a[1]); }
  ///@}

  /**
   * Get the current size of the screen in pixels.
   * An HDTV for example would be 1920 x 1080 pixels.
   */
  int* GetScreenSize() VTK_SIZEHINT(2) override;

  /**
   * Get the position (x and y) of the rendering window in
   * screen coordinates (in pixels).
   */
  int* GetPosition() VTK_SIZEHINT(2) override;

  /**
   * Set the name of the window. This appears at the top of the window
   * normally.
   */
  void SetWindowName(const char*) override;

  /**
   * Set the icon displayed in the title bar and the taskbar.
   */
  void SetIcon(vtkImageData* img) override;

  /**
   * Set this RenderWindow's window id to a pre-existing window.
   */
  void SetWindowInfo(const char*) override;

  /**
   * Sets the WindowInfo that will be used after a WindowRemap.
   */
  void SetNextWindowInfo(const char*) override;

  /**
   * Sets the HWND id of the window that WILL BE created.
   */
  void SetParentInfo(const char*) override;

  void* GetGenericDisplayId() override { return (void*)this->DeviceContext; }
  void* GetGenericWindowId() override { return (void*)this->WindowId; }
  void* GetGenericParentId() override { return (void*)this->ParentId; }
  void* GetGenericDrawable() override { return (void*)this->WindowId; }
  void SetDisplayId(void*) override;

  /**
   * Get the window id.
   */
  HWND GetWindowId();

  ///@{
  /**
   * Set the window id to a pre-existing window.
   */
  void SetWindowId(HWND);
  void SetWindowId(void* foo) override { this->SetWindowId((HWND)foo); }
  ///@}

  /**
   * Initialize the render window from the information associated
   * with the currently activated OpenGL context.
   */
  bool InitializeFromCurrentContext() override;

  /**
   * Does this platform support render window data sharing.
   */
  bool GetPlatformSupportsRenderWindowSharing() override { return true; }

  ///@{
  /**
   * Set the window's parent id to a pre-existing window.
   */
  void SetParentId(HWND);
  void SetParentId(void* foo) override { this->SetParentId((HWND)foo); }
  ///@}

  void SetContextId(HGLRC);   // hsr
  void SetDeviceContext(HDC); // hsr

  /**
   * Set the window id of the new window once a WindowRemap is done.
   */
  void SetNextWindowId(HWND);

  /**
   * Set the window id of the new window once a WindowRemap is done.
   * This is the generic prototype as required by the vtkRenderWindow
   * parent.
   */
  void SetNextWindowId(void* arg) override;

  /**
   * Prescribe that the window be created in a stereo-capable mode. This
   * method must be called before the window is realized. This method
   * overrides the superclass method since this class can actually check
   * whether the window has been realized yet.
   */
  void SetStereoCapableWindow(vtkTypeBool capable) override;

  /**
   * Get report of capabilities for the render window
   */
  const char* ReportCapabilities() override;

  /**
   * Is this render window using hardware acceleration? 0-false, 1-true
   */
  vtkTypeBool IsDirect() override;

  /**
   * Check to see if a mouse button has been pressed or mouse wheel activated.
   * All other events are ignored by this method.
   * This is a useful check to abort a long render.
   */
  vtkTypeBool GetEventPending() override;

  ///@{
  /**
   * Hide or Show the mouse cursor, it is nice to be able to hide the
   * default cursor if you want VTK to display a 3D cursor instead.
   * Set cursor position in window (note that (0,0) is the lower left
   * corner).
   */
  void HideCursor() override;
  void ShowCursor() override;
  void SetCursorPosition(int x, int y) override;
  ///@}

  /**
   * Change the shape of the cursor
   */
  void SetCurrentCursor(int) override;

  bool DetectDPI() override;
protected:
  vtkWin32WebGPURenderWindow();
  ~vtkWin32WebGPURenderWindow();

  // Helper members
  HINSTANCE ApplicationInstance;
  HDC DeviceContext;
  BOOL MFChandledWindow;
  HWND WindowId;
  HWND ParentId;
  HWND NextWindowId;
  vtkTypeBool OwnWindow;
  vtkTypeBool Resizing;
  vtkTypeBool Repositioning;
  static const std::string DEFAULT_BASE_WINDOW_NAME;

  std::stack<HDC> DCStack;

  // message handler
  virtual LRESULT MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  static LRESULT APIENTRY WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  vtkTypeBool CursorHidden;
  vtkTypeBool ForceMakeCurrent;

  int WindowIdReferenceCount;
  void ResizeWhileOffscreen(int xsize, int ysize);
  void CreateAWindow() override;
  void DestroyWindow() override;
  void InitializeApplication();
  void CleanUpRenderers();
  void VTKRegisterClass();

private:
  vtkWin32WebGPURenderWindow(const vtkWin32WebGPURenderWindow&) = delete;
  void operator=(const vtkWin32WebGPURenderWindow) = delete;
};

VTK_ABI_NAMESPACE_END
#endif //vtkWin32WebGPURenderWindow_h
