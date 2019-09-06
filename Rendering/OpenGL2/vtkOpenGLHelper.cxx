/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkOpenGLHelper.h"

#include "vtkOpenGLIndexBufferObject.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLVertexArrayObject.h"
#include "vtkShaderProgram.h"

vtkOpenGLHelper::vtkOpenGLHelper()
{
  this->Program = nullptr;
  this->IBO = vtkOpenGLIndexBufferObject::New();
  this->VAO = vtkOpenGLVertexArrayObject::New();
  this->ShaderChangeValue = 0;
}

vtkOpenGLHelper::~vtkOpenGLHelper()
{
  this->IBO->Delete();
  this->VAO->Delete();
}

void vtkOpenGLHelper::ReleaseGraphicsResources(vtkWindow * win)
{
  vtkOpenGLRenderWindow *rwin =
   vtkOpenGLRenderWindow::SafeDownCast(win);
  if (rwin)
  {
    // Ensure that the context is current before releasing any
    // graphics resources tied to it.
    rwin->MakeCurrent();
  }

  if (this->Program)
  {
    // Let ShaderCache release the graphics resources as it is
    // responsible for creation and deletion.
    this->Program = nullptr;
  }
  this->IBO->ReleaseGraphicsResources();
  this->VAO->ReleaseGraphicsResources();
}
