/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkClearRGBPass.h"
#include "vtkObjectFactory.h"
#include "vtkRenderState.h"
#include "vtkOpenGLState.h"
#include "vtkOpenGLRenderer.h"
#include "vtk_glew.h"

vtkStandardNewMacro(vtkClearRGBPass);

// ----------------------------------------------------------------------------
vtkClearRGBPass::vtkClearRGBPass()
{
  this->Background[0] = 0;
  this->Background[1] = 0;
  this->Background[2] = 0;
}

// ----------------------------------------------------------------------------
vtkClearRGBPass::~vtkClearRGBPass() = default;

// ----------------------------------------------------------------------------
void vtkClearRGBPass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Background:"
     << this->Background[0] << "," << this->Background[1] << ","
     << this->Background[2] << endl;
}

// ----------------------------------------------------------------------------
void vtkClearRGBPass::Render(const vtkRenderState *s)
{
  this->NumberOfRenderedProps=0;

  vtkOpenGLState *ostate =
    static_cast<vtkOpenGLRenderer*>(s->GetRenderer())->GetState();
  ostate->vtkglClearColor( static_cast<GLclampf>(this->Background[0]),
                static_cast<GLclampf>(this->Background[1]),
                static_cast<GLclampf>(this->Background[2]),
                static_cast<GLclampf>(0.0));
  ostate->vtkglClear(GL_COLOR_BUFFER_BIT);
}
