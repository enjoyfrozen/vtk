/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// We never need to instantiate vtkAOSDataArrayTemplate<vtkIdType> or
// vtkArrayIteratorTemplate<vtkIdType> because they are instantiated
// by the corresponding array for its native type.  Therefore this
// code should not be uncommented and is here for reference:
//   #define VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATING
//   #include "vtkAOSDataArrayTemplate.txx"
//   VTK_AOS_DATA_ARRAY_TEMPLATE_INSTANTIATE(vtkIdType);

#include "vtkIdTypeArray.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkIdTypeArray);

//----------------------------------------------------------------------------
vtkIdTypeArray::vtkIdTypeArray() = default;

//----------------------------------------------------------------------------
vtkIdTypeArray::~vtkIdTypeArray() = default;

//----------------------------------------------------------------------------
void vtkIdTypeArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->RealSuperclass::PrintSelf(os,indent);
}
