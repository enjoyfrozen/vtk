/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkmWarpScalar
 * @brief   deform geometry with scalar data
 *
 * vtkmWarpScalar is a filter that modifies point coordinates by moving points
 * along point normals by the scalar amount times the scalar factor with vtkm
 * as its backend.
 * Useful for creating carpet or x-y-z plots.
 *
 * If normals are not present in data, the Normal instance variable will
 * be used as the direction along which to warp the geometry. If normals are
 * present but you would like to use the Normal instance variable, set the
 * UseNormal boolean to true.
 *
 * If XYPlane boolean is set true, then the z-value is considered to be
 * a scalar value (still scaled by scale factor), and the displacement is
 * along the z-axis. If scalars are also present, these are copied through
 * and can be used to color the surface.
 *
 * Note that the filter passes both its point data and cell data to
 * its output, except for normals, since these are distorted by the
 * warping.
*/

#ifndef vtkmWarpScalar_h
#define vtkmWarpScalar_h

#include "vtkWarpScalar.h"
#include "vtkAcceleratorsVTKmModule.h" // required for correct export

class VTKACCELERATORSVTKM_EXPORT vtkmWarpScalar : public vtkWarpScalar
{
public:
  vtkTypeMacro(vtkmWarpScalar, vtkWarpScalar)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkmWarpScalar* New();

protected:
  vtkmWarpScalar();
  ~vtkmWarpScalar();

  int RequestData(vtkInformation* , vtkInformationVector**,
                          vtkInformationVector*) override;

private:
  vtkmWarpScalar(const vtkmWarpScalar&) = delete;
  void operator=(const vtkmWarpScalar&) = delete;
};

#endif // vtkmWarpScalar_h

// VTK-HeaderTest-Exclude: vtkmWarpScalar.h
