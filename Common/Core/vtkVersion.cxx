/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkVersion.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkVersion);
const char* GetVTKVersion()
{
  return vtkVersion::GetVTKVersion();
}
