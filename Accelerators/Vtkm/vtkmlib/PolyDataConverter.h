/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#ifndef vtkmlib_PolyDataConverter_h
#define vtkmlib_PolyDataConverter_h

#include "vtkAcceleratorsVTKmModule.h"

#include "ArrayConverters.h" // for FieldsFlag

#include "vtkmConfig.h" //required for general vtkm setup

#include <vtkm/cont/DataSet.h>

class vtkPolyData;
class vtkDataSet;

namespace tovtkm {
// convert an polydata type
VTKACCELERATORSVTKM_EXPORT
vtkm::cont::DataSet Convert(vtkPolyData* input,
                            FieldsFlag fields = FieldsFlag::None);
}

namespace fromvtkm {
VTKACCELERATORSVTKM_EXPORT
bool Convert(const vtkm::cont::DataSet& voutput, vtkPolyData* output,
             vtkDataSet* input);
}
#endif // vtkmlib_PolyDataConverter_h
