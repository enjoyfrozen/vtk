/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOpenGLLight
 * @brief   OpenGL light
 *
 * vtkOpenGLLight is a concrete implementation of the abstract class vtkLight.
 * vtkOpenGLLight interfaces to the OpenGL rendering library.
*/

#ifndef vtkOpenGLLight_h
#define vtkOpenGLLight_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkLight.h"

class vtkOpenGLRenderer;

class VTKRENDERINGOPENGL2_EXPORT vtkOpenGLLight : public vtkLight
{
public:
  static vtkOpenGLLight *New();
  vtkTypeMacro(vtkOpenGLLight, vtkLight);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Implement base class method.
   */
  void Render(vtkRenderer *ren, int light_index) override;

protected:
  vtkOpenGLLight() {}
  ~vtkOpenGLLight() override {}

private:
  vtkOpenGLLight(const vtkOpenGLLight&) = delete;
  void operator=(const vtkOpenGLLight&) = delete;
};

#endif
