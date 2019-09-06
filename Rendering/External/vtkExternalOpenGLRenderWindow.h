/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkExternalOpenGLRenderWindow
 * @brief   OpenGL render window that allows using
 * an external window to render vtk objects
 *
 * vtkExternalOpenGLRenderWindow is a concrete implementation of the abstract
 * class vtkRenderWindow. vtkExternalOpenGLRenderer interfaces to the OpenGL
 * graphics library.
 *
 * This class extends vtkGenericOpenGLRenderWindow to allow sharing the
 * same OpenGL context by various visualization applications. Basically, this
 * class prevents VTK from creating a new OpenGL context. Thus, it requires that
 * an OpenGL context be initialized before Render is called.
 * \sa Render()
 *
 * It is a generic implementation; this window is platform agnostic. However,
 * the application user must explicitly make sure the window size is
 * synchronized when the external application window/viewport resizes.
 * \sa SetSize()
 *
 * It has the same requirements as the vtkGenericOpenGLRenderWindow, whereby,
 * one must register an observer for WindowMakeCurrentEvent,
 * WindowIsCurrentEvent and WindowFrameEvent.
 * \sa vtkGenericOpenGLRenderWindow
*/

#ifndef vtkExternalOpenGLRenderWindow_h
#define vtkExternalOpenGLRenderWindow_h

#include "vtkRenderingExternalModule.h" // For export macro
#include "vtkGenericOpenGLRenderWindow.h"

class VTKRENDERINGEXTERNAL_EXPORT vtkExternalOpenGLRenderWindow :
  public vtkGenericOpenGLRenderWindow
{
public:
  static vtkExternalOpenGLRenderWindow *New();
  vtkTypeMacro(vtkExternalOpenGLRenderWindow, vtkGenericOpenGLRenderWindow);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  /**
   * Begin the rendering process using the existing context.
   */
  void Start(void) override;

  /**
   * Tells if this window is the current graphics context for the calling
   * thread.
   */
  bool IsCurrent() override;

  //@{
  /**
   * Turn on/off a flag which enables/disables automatic positioning and
   * resizing of the render window. By default, vtkExternalOpenGLRenderWindow
   * queries the viewport position and size (glViewport) from the OpenGL state
   * and uses it to resize itself. However, in special circumstances this
   * feature is undesirable. One such circumstance may be to avoid performance
   * penalty of querying OpenGL state variables. So the following boolean is
   * provided to disable automatic window resize.
   * (Turn AutomaticWindowPositionAndResize off if you do not want the viewport
   * to be queried from the OpenGL state.)
   */
  vtkGetMacro(AutomaticWindowPositionAndResize,int);
  vtkSetMacro(AutomaticWindowPositionAndResize,int);
  vtkBooleanMacro(AutomaticWindowPositionAndResize,int);
  //@}

protected:
  vtkExternalOpenGLRenderWindow();
  ~vtkExternalOpenGLRenderWindow() override;

  int AutomaticWindowPositionAndResize;

private:
  vtkExternalOpenGLRenderWindow(const vtkExternalOpenGLRenderWindow&) = delete;
  void operator=(const vtkExternalOpenGLRenderWindow&) = delete;
};
#endif //vtkExternalOpenGLRenderWindow_h
