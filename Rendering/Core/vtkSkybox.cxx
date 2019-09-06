/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkSkybox.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"

//------------------------------------------------------------------------------
vtkObjectFactoryNewMacro(vtkSkybox)

//------------------------------------------------------------------------------
void vtkSkybox::PrintSelf(std::ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
// Always return uninitialized
double *vtkSkybox::GetBounds()
{
  return nullptr;
}

//------------------------------------------------------------------------------
vtkSkybox::vtkSkybox()
{
  this->Projection = vtkSkybox::Cube;
  this->FloorPlane[0] = 0.0;
  this->FloorPlane[1] = 1.0;
  this->FloorPlane[2] = 0.0;
  this->FloorPlane[3] = 0.0;
  this->FloorRight[0] = 1.0;
  this->FloorRight[1] = 0.0;
  this->FloorRight[2] = 0.0;
}

//------------------------------------------------------------------------------
vtkSkybox::~vtkSkybox() = default;
