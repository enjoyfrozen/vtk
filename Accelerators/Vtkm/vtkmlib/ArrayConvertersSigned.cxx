/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#define vtkmlib_ArrayConverterExport_cxx
#include "ArrayConverters.hxx"

namespace tovtkm
{

VTK_EXPORT_SIGNED_ARRAY_CONVERSION_TO_VTKM(vtkAOSDataArrayTemplate)
VTK_EXPORT_SIGNED_ARRAY_CONVERSION_TO_VTKM(vtkSOADataArrayTemplate)

} // tovtkm
