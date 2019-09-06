/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#ifndef vtkmlib_ImageDataConverter_h
#define vtkmlib_ImageDataConverter_h

#include "vtkAcceleratorsVTKmModule.h"

#include "ArrayConverters.h" // for FieldsFlag

#include "vtkmConfig.h" //required for general vtkm setup

#include <vtkm/cont/DataSet.h>

class vtkImageData;
class vtkDataSet;

namespace tovtkm {

VTKACCELERATORSVTKM_EXPORT
vtkm::cont::DataSet Convert(vtkImageData* input,
                            FieldsFlag fields = FieldsFlag::None);

}

namespace fromvtkm {

VTKACCELERATORSVTKM_EXPORT
bool Convert(const vtkm::cont::DataSet& voutput, vtkImageData* output, vtkDataSet* input);

VTKACCELERATORSVTKM_EXPORT
bool Convert(const vtkm::cont::DataSet& voutput, int extents[6], vtkImageData* output,
             vtkDataSet* input);

}
#endif // vtkmlib_ImageDataConverter_h
