/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#define vtkmDataArray_cxx

#include "vtkmDataArray.h"

template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<char>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<double>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<float>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<int>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<long>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<long long>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<short>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<signed char>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<unsigned char>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<unsigned int>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<unsigned long>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<unsigned long long>;
template class VTKACCELERATORSVTKM_EXPORT vtkmDataArray<unsigned short>;
