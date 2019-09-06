/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkStructuredExtent.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkStructuredExtent);
//----------------------------------------------------------------------------
vtkStructuredExtent::vtkStructuredExtent() = default;

//----------------------------------------------------------------------------
vtkStructuredExtent::~vtkStructuredExtent() = default;

//----------------------------------------------------------------------------
void vtkStructuredExtent::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
