/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkOpenGLTextActor3D
 * @brief   OpenGL2 override for vtkTextActor3D.
*/

#ifndef vtkOpenGLTextActor3D_h
#define vtkOpenGLTextActor3D_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkTextActor3D.h"

class vtkOpenGLGL2PSHelper;

class VTKRENDERINGOPENGL2_EXPORT vtkOpenGLTextActor3D: public vtkTextActor3D
{
public:
  static vtkOpenGLTextActor3D* New();
  vtkTypeMacro(vtkOpenGLTextActor3D, vtkTextActor3D)
  void PrintSelf(ostream &os, vtkIndent indent) override;

  int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;

protected:
  vtkOpenGLTextActor3D();
  ~vtkOpenGLTextActor3D() override;

  int RenderGL2PS(vtkViewport *vp, vtkOpenGLGL2PSHelper *gl2ps);

private:
  vtkOpenGLTextActor3D(const vtkOpenGLTextActor3D&) = delete;
  void operator=(const vtkOpenGLTextActor3D&) = delete;
};

#endif // vtkOpenGLTextActor3D_h
