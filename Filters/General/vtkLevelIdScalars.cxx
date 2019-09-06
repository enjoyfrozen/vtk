/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkLevelIdScalars.h"
#include "vtkObjectFactory.h"


vtkStandardNewMacro(vtkLevelIdScalars);

//------------------------------------------------------------------------------
vtkLevelIdScalars::vtkLevelIdScalars() = default;

//------------------------------------------------------------------------------
vtkLevelIdScalars::~vtkLevelIdScalars() = default;

//------------------------------------------------------------------------------
void vtkLevelIdScalars::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
