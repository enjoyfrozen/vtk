/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmCoordinateSystemTransform
 * @brief   transform a coordinate system between Cartesian&Cylindrical and
 *          Cartesian&Spherical
 *
 * vtkmCoordinateSystemTransform is a filter that transforms a coordinate system
 * between Cartesian&Cylindrical and Cartesian&Spherical.
*/

#ifndef vtkmCoordinateSystemTransform_h
#define vtkmCoordinateSystemTransform_h

#include "vtkPointSetAlgorithm.h"
#include "vtkAcceleratorsVTKmModule.h" // required for correct export

class VTKACCELERATORSVTKM_EXPORT vtkmCoordinateSystemTransform : public vtkPointSetAlgorithm
{
  enum struct TransformTypes {None, CarToCyl, CylToCar, CarToSph, SphToCar};
public:
  vtkTypeMacro(vtkmCoordinateSystemTransform, vtkPointSetAlgorithm)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkmCoordinateSystemTransform* New();

  void SetCartesianToCylindrical();
  void SetCylindricalToCartesian();

  void SetCartesianToSpherical();
  void SetSphericalToCartesian();

  int FillInputPortInformation(int port, vtkInformation* info) override;
protected:
  vtkmCoordinateSystemTransform();
  ~vtkmCoordinateSystemTransform();

  int RequestDataObject(vtkInformation* request,
                        vtkInformationVector** inputVector,
                        vtkInformationVector* outputVector) override;
  int RequestData(vtkInformation* , vtkInformationVector**,
                          vtkInformationVector*) override;

private:
  vtkmCoordinateSystemTransform(const vtkmCoordinateSystemTransform&) = delete;
  void operator=(const vtkmCoordinateSystemTransform&) = delete;

  TransformTypes TransformType;
};

#endif // vtkmCoordinateSystemTransform_h

// VTK-HeaderTest-Exclude: vtkmCoordinateSystemTransform.h
