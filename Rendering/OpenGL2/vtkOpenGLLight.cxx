/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkOpenGLLight.h"

#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkOpenGLLight);

// Implement base class method.
void vtkOpenGLLight::Render(vtkRenderer *vtkNotUsed(ren), int vtkNotUsed(light_index))
{
  // all handled by the mappers
}

//----------------------------------------------------------------------------
void vtkOpenGLLight::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
