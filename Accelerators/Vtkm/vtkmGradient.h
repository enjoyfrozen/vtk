/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmGradient
 * @brief   A general filter for gradient estimation.
 *
 * Estimates the gradient of a field in a data set.  The gradient calculation
 * is dependent on the input dataset type.  The created gradient array
 * is of the same type as the array it is calculated from (e.g. point data
 * or cell data) as well as data type (e.g. float, double). The output array has
 * 3*number of components of the input data array.  The ordering for the
 * output tuple will be {du/dx, du/dy, du/dz, dv/dx, dv/dy, dv/dz, dw/dx,
 * dw/dy, dw/dz} for an input array {u, v, w}.
 *
 * Also options to additionally compute the divergence, vorticity and
 * Q criterion of input vector fields.
 *
*/

#ifndef vtkmGradient_h
#define vtkmGradient_h

#include "vtkAcceleratorsVTKmModule.h" //required for correct implementation
#include "vtkGradientFilter.h"

class VTKACCELERATORSVTKM_EXPORT vtkmGradient : public vtkGradientFilter
{
public:
  vtkTypeMacro(vtkmGradient, vtkGradientFilter)
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkmGradient* New();

protected:
  vtkmGradient();
  ~vtkmGradient();


  virtual int RequestData(vtkInformation*, vtkInformationVector**,
                          vtkInformationVector*) override;

private:
  vtkmGradient(const vtkmGradient&) = delete;
  void operator=(const vtkmGradient&) = delete;
};

#endif // vtkmGradient_h
// VTK-HeaderTest-Exclude: vtkmGradient.h
