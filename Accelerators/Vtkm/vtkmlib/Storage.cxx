/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#define vtkmlib_Storage_cxx
#include "Storage.h"

namespace vtkm
{
namespace cont
{
namespace internal
{

// T extern template instantiations
VTKM_TEMPLATE_IMPORT_Storage(char, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::Int8, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::UInt8, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::Int16, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::UInt16, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::Int32, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::UInt32, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::Int64, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::UInt64, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::Float32, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(vtkm::Float64, tovtkm::vtkSOAArrayContainerTag);

#if VTKM_SIZE_LONG_LONG == 8
VTKM_TEMPLATE_IMPORT_Storage(long, tovtkm::vtkSOAArrayContainerTag);
VTKM_TEMPLATE_IMPORT_Storage(unsigned long, tovtkm::vtkSOAArrayContainerTag);
#endif

template class VTKACCELERATORSVTKM_EXPORT
    Storage<vtkIdType, tovtkm::vtkCellArrayContainerTag>;
}
}
}
