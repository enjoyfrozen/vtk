/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkOpenGLTextActor
 * @brief   vtkTextActor override.
*/

#ifndef vtkOpenGLTextActor_h
#define vtkOpenGLTextActor_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkTextActor.h"

class vtkOpenGLGL2PSHelper;

class VTKRENDERINGOPENGL2_EXPORT vtkOpenGLTextActor: public vtkTextActor
{
public:
  static vtkOpenGLTextActor* New();
  vtkTypeMacro(vtkOpenGLTextActor, vtkTextActor)
  void PrintSelf(ostream &os, vtkIndent indent) override;

  int RenderOverlay(vtkViewport* viewport) override;

protected:
  vtkOpenGLTextActor();
  ~vtkOpenGLTextActor() override;

  int RenderGL2PS(vtkViewport *viewport, vtkOpenGLGL2PSHelper *gl2ps);

private:
  vtkOpenGLTextActor(const vtkOpenGLTextActor&) = delete;
  void operator=(const vtkOpenGLTextActor&) = delete;
};

#endif // vtkOpenGLTextActor_h
