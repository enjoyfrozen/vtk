/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkContextDevice3D.h"
#include "vtkObjectFactory.h"

vtkAbstractObjectFactoryNewMacro(vtkContextDevice3D)

vtkContextDevice3D::vtkContextDevice3D() = default;

vtkContextDevice3D::~vtkContextDevice3D() = default;

//-----------------------------------------------------------------------------
void vtkContextDevice3D::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
