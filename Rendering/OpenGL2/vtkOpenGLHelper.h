/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkOpenGLHelper_h
#define vtkOpenGLHelper_h

#include "vtkRenderingOpenGL2Module.h" // for export macro
#include "vtkTimeStamp.h"

class vtkOpenGLIndexBufferObject;
class vtkOpenGLShaderCache;
class vtkOpenGLVertexArrayObject;
class vtkShaderProgram;
class vtkWindow;

// Store the shaders, program, and ibo in a common place
// as they are used together frequently.  This is just
// a convenience class.
class VTKRENDERINGOPENGL2_EXPORT vtkOpenGLHelper
{
public:
  vtkShaderProgram *Program;
  vtkTimeStamp ShaderSourceTime;
  vtkOpenGLVertexArrayObject *VAO;
  vtkTimeStamp AttributeUpdateTime;
  int PrimitiveType;
  unsigned int ShaderChangeValue;

  vtkOpenGLIndexBufferObject *IBO;

  vtkOpenGLHelper();
  ~vtkOpenGLHelper();
  void ReleaseGraphicsResources(vtkWindow *win);

 private:
  vtkOpenGLHelper(const vtkOpenGLHelper&) = delete;
  vtkOpenGLHelper& operator=(const vtkOpenGLHelper&) = delete;
};

#endif // vtkOpenGLHelper_h

// VTK-HeaderTest-Exclude: vtkOpenGLHelper.h
