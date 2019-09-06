/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#ifndef vtkmlib_ImplicitFunctionConverter_h
#define vtkmlib_ImplicitFunctionConverter_h

#include "vtkAcceleratorsVTKmModule.h"
#include "vtkmConfig.h" //required for general vtkm setup
#include "vtkType.h" // For vtkMTimeType

#include "vtkm/cont/ImplicitFunctionHandle.h"

class vtkImplicitFunction;

namespace tovtkm {

class VTKACCELERATORSVTKM_EXPORT ImplicitFunctionConverter
{
public:
  ImplicitFunctionConverter();

  void Set(vtkImplicitFunction *);
  const vtkm::cont::ImplicitFunctionHandle& Get() const;

private:
  vtkImplicitFunction *InFunction;
  vtkm::cont::ImplicitFunctionHandle OutFunction;
  mutable vtkMTimeType MTime;
};

}

#endif // vtkmlib_ImplicitFunctionConverter_h
