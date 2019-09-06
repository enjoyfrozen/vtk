/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkUnstructuredGridVolumeRayIntegrator
 * @brief   a superclass for volume ray integration functions
 *
 *
 *
 * vtkUnstructuredGridVolumeRayIntegrator is a superclass for ray
 * integration functions that can be used within a
 * vtkUnstructuredGridVolumeRayCastMapper.
 *
 * @sa
 * vtkUnstructuredGridVolumeRayCastMapper
 * vtkUnstructuredGridVolumeRayCastFunction
*/

#ifndef vtkUnstructuredGridVolumeRayIntegrator_h
#define vtkUnstructuredGridVolumeRayIntegrator_h

#include "vtkRenderingVolumeModule.h" // For export macro
#include "vtkObject.h"

class vtkVolume;
class vtkDoubleArray;
class vtkDataArray;

class VTKRENDERINGVOLUME_EXPORT vtkUnstructuredGridVolumeRayIntegrator : public vtkObject
{
public:
  vtkTypeMacro(vtkUnstructuredGridVolumeRayIntegrator,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Set up the integrator with the given properties and scalars.
   */
  virtual void Initialize(vtkVolume *volume,
                          vtkDataArray* scalars) = 0;

  /**
   * Given a set of intersections (defined by the three arrays), compute
   * the piecewise integration of the array in front to back order.
   * /c intersectionLengths holds the lengths of each piecewise segment.
   * /c nearIntersections and /c farIntersections hold the scalar values at
   * the front and back of each segment.  /c color should contain the RGBA
   * value of the volume in front of the segments passed in, and the result
   * will be placed back into /c color.
   */
  virtual void Integrate(vtkDoubleArray *intersectionLengths,
                         vtkDataArray *nearIntersections,
                         vtkDataArray *farIntersections,
                         float color[4]) = 0;

protected:
  vtkUnstructuredGridVolumeRayIntegrator();
  ~vtkUnstructuredGridVolumeRayIntegrator() override;

private:
  vtkUnstructuredGridVolumeRayIntegrator(const vtkUnstructuredGridVolumeRayIntegrator&) = delete;
  void operator=(const vtkUnstructuredGridVolumeRayIntegrator&) = delete;
};

#endif

