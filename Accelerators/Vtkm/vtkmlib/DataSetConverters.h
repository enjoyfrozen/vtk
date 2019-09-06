/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#ifndef vtkmlib_DataSetConverters_h
#define vtkmlib_DataSetConverters_h

#include "vtkAcceleratorsVTKmModule.h"

#include "ArrayConverters.h" // for FieldsFlag

#include "vtkmConfig.h" //required for general vtkm setup

#include <vtkm/cont/DataSet.h>

class vtkImageData;
class vtkStructuredGrid;
class vtkPoints;
class vtkDataSet;

namespace tovtkm {

// convert a vtkPoints array into a coordinate system
VTKACCELERATORSVTKM_EXPORT
vtkm::cont::CoordinateSystem Convert(vtkPoints* points);

// convert an structured grid type
VTKACCELERATORSVTKM_EXPORT
vtkm::cont::DataSet Convert(vtkStructuredGrid* input,
                            FieldsFlag fields = FieldsFlag::None);


// determine the type and call the proper Convert routine
VTKACCELERATORSVTKM_EXPORT
vtkm::cont::DataSet Convert(vtkDataSet* input,
                            FieldsFlag fields = FieldsFlag::None);
}

#endif // vtkmlib_DataSetConverters_h
