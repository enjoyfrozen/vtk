/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkFixedPointVolumeRayCastHelper.h"
#include "vtkObjectFactory.h"

#include <cmath>

vtkStandardNewMacro(vtkFixedPointVolumeRayCastHelper);

vtkFixedPointVolumeRayCastHelper::vtkFixedPointVolumeRayCastHelper() = default;

vtkFixedPointVolumeRayCastHelper::~vtkFixedPointVolumeRayCastHelper() = default;

void vtkFixedPointVolumeRayCastHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


