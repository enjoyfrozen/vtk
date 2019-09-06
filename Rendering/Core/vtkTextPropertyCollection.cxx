/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkTextPropertyCollection.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTextPropertyCollection)

//----------------------------------------------------------------------------
void vtkTextPropertyCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
vtkTextPropertyCollection::vtkTextPropertyCollection() = default;

//----------------------------------------------------------------------------
vtkTextPropertyCollection::~vtkTextPropertyCollection() = default;
