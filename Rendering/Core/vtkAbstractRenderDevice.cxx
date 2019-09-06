/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAbstractRenderDevice.h"
#include "vtkObjectFactory.h"

vtkAbstractObjectFactoryNewMacro(vtkAbstractRenderDevice)

vtkAbstractRenderDevice::vtkAbstractRenderDevice() : GLMajor(2), GLMinor(1)
{
}

vtkAbstractRenderDevice::~vtkAbstractRenderDevice() = default;

void vtkAbstractRenderDevice::SetRequestedGLVersion(int major, int minor)
{
  this->GLMajor = major;
  this->GLMinor = minor;
}

void vtkAbstractRenderDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
