/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// Instantiate superclass first to give the template a DLL interface.
#define VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATING
#include "vtkAOSDataArrayTemplate.txx"
VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATE(int);

#include "vtkIntArray.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIntArray);

//----------------------------------------------------------------------------
vtkIntArray::vtkIntArray() = default;

//----------------------------------------------------------------------------
vtkIntArray::~vtkIntArray() = default;

//----------------------------------------------------------------------------
void vtkIntArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->RealSuperclass::PrintSelf(os,indent);
}
