/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkRayCastImageDisplayHelper.h"
#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
// Return nullptr if no override is supplied.
vtkAbstractObjectFactoryNewMacro(vtkRayCastImageDisplayHelper)
//----------------------------------------------------------------------------

// Construct a new vtkRayCastImageDisplayHelper with default values
vtkRayCastImageDisplayHelper::vtkRayCastImageDisplayHelper()
{
  this->PreMultipliedColors = 1;
  this->PixelScale = 1.0;
}

// Destruct a vtkRayCastImageDisplayHelper - clean up any memory used
vtkRayCastImageDisplayHelper::~vtkRayCastImageDisplayHelper() = default;

void vtkRayCastImageDisplayHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "PreMultiplied Colors: "
     << (this->PreMultipliedColors ? "On" : "Off") << endl;

  os << indent << "Pixel Scale: " << this->PixelScale << endl;
}
