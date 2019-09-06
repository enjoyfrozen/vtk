/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#ifndef vtkmlib_CellSetConverters_h
#define vtkmlib_CellSetConverters_h

#include "vtkAcceleratorsVTKmModule.h"
#include "vtkmConfig.h" //required for general vtkm setup

#include <vtkm/cont/DynamicCellSet.h>
#include <vtkType.h>

class vtkCellArray;
class vtkUnsignedCharArray;
class vtkIdTypeArray;

namespace tovtkm {
VTKACCELERATORSVTKM_EXPORT
vtkm::cont::DynamicCellSet ConvertSingleType(vtkCellArray* cells, int cellType,
                                             vtkIdType numberOfPoints);

VTKACCELERATORSVTKM_EXPORT
vtkm::cont::DynamicCellSet Convert(vtkUnsignedCharArray* types,
                                   vtkCellArray* cells,
                                   vtkIdTypeArray* locations,
                                   vtkIdType numberOfPoints);
}

namespace fromvtkm {

VTKACCELERATORSVTKM_EXPORT
bool Convert(const vtkm::cont::DynamicCellSet& toConvert,
             vtkCellArray* cells,
             vtkUnsignedCharArray* types = nullptr,
             vtkIdTypeArray* locations = nullptr);
}

#endif // vtkmlib_CellSetConverters_h
