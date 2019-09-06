/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAbstractInteractionDevice.h"

#include "vtkObjectFactory.h"

vtkAbstractObjectFactoryNewMacro(vtkAbstractInteractionDevice)

vtkAbstractInteractionDevice::vtkAbstractInteractionDevice()
  : Initialized(false), RenderWidget(nullptr), RenderDevice(nullptr)
{
}

vtkAbstractInteractionDevice::~vtkAbstractInteractionDevice() = default;

void vtkAbstractInteractionDevice::SetRenderWidget(vtkRenderWidget *widget)
{
  if (this->RenderWidget != widget)
  {
    this->RenderWidget = widget;
    this->Modified();
  }
}

void vtkAbstractInteractionDevice::SetRenderDevice(vtkAbstractRenderDevice *d)
{
  if (this->RenderDevice != d)
  {
    this->RenderDevice = d;
    this->Modified();
  }
}

void vtkAbstractInteractionDevice::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
