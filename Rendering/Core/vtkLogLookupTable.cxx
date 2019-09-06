/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkLogLookupTable.h"
#include "vtkObjectFactory.h"

#include <cmath>

vtkStandardNewMacro(vtkLogLookupTable);

// Construct with (minimum,maximum) range 1 to 10 (based on
// logarithmic values).
vtkLogLookupTable::vtkLogLookupTable(int sze, int ext)
  : vtkLookupTable(sze,ext)
{
  this->Scale = VTK_SCALE_LOG10;

  this->TableRange[0] = 1;
  this->TableRange[1] = 10;
}

void vtkLogLookupTable::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
