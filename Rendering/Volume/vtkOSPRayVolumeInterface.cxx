/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkOSPRayVolumeInterface.h"

#include "vtkObjectFactory.h"

vtkObjectFactoryNewMacro(vtkOSPRayVolumeInterface);

// ----------------------------------------------------------------------------
vtkOSPRayVolumeInterface::vtkOSPRayVolumeInterface() = default;

// ----------------------------------------------------------------------------
vtkOSPRayVolumeInterface::~vtkOSPRayVolumeInterface() = default;

// ----------------------------------------------------------------------------
void vtkOSPRayVolumeInterface::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

// ----------------------------------------------------------------------------
void vtkOSPRayVolumeInterface::Render(vtkRenderer *vtkNotUsed(ren),
                                            vtkVolume *vtkNotUsed(vol))
{
  cerr
    << "Warning VTK is not linked to OSPRay so can not VolumeRender with it"
    << endl;
}
