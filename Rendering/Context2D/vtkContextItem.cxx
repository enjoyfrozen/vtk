/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkContextItem.h"

//-----------------------------------------------------------------------------
vtkContextItem::vtkContextItem()
{
  this->Opacity = 1.0;
}

//-----------------------------------------------------------------------------
vtkContextItem::~vtkContextItem() = default;

//-----------------------------------------------------------------------------
void vtkContextItem::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
