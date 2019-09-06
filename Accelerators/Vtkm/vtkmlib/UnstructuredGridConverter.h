/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#ifndef vtkmlib_UnstructuredGridConverter_h
#define vtkmlib_UnstructuredGridConverter_h

#include "vtkAcceleratorsVTKmModule.h"

#include "ArrayConverters.h" // For FieldsFlag

#include "vtkmConfig.h" //required for general vtkm setup

#include <vtkm/cont/DataSet.h>

class vtkUnstructuredGrid;
class vtkDataSet;

namespace tovtkm {

// convert an unstructured grid type
VTKACCELERATORSVTKM_EXPORT
vtkm::cont::DataSet Convert(vtkUnstructuredGrid* input,
                            FieldsFlag fields = FieldsFlag::None);
}

namespace fromvtkm {
VTKACCELERATORSVTKM_EXPORT
bool Convert(const vtkm::cont::DataSet& voutput, vtkUnstructuredGrid* output,
             vtkDataSet* input);
}
#endif // vtkmlib_UnstructuredGridConverter_h
