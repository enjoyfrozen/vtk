/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkGeoMath
 * @brief   Useful geographic calculations
 *
 *
 * vtkGeoMath provides some useful geographic calculations.
*/

#ifndef vtkGeoMath_h
#define vtkGeoMath_h

#include "vtkInfovisLayoutModule.h" // For export macro
#include "vtkObject.h"

class VTKINFOVISLAYOUT_EXPORT vtkGeoMath : public vtkObject
{
public:
  static vtkGeoMath *New();
  vtkTypeMacro(vtkGeoMath, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Returns the average radius of the earth in meters.
   */
  static double EarthRadiusMeters() {return 6356750.0;}

  /**
   * Returns the squared distance between two points.
   */
  static double DistanceSquared(double pt0[3], double pt1[3]);

  /**
   * Converts a (longitude, latitude, altitude) triple to
   * world coordinates where the center of the earth is at the origin.
   * Units are in meters.
   * Note that having altitude realtive to sea level causes issues.
   */
  static void   LongLatAltToRect(double lla[3], double rect[3]);

protected:
  vtkGeoMath();
  ~vtkGeoMath() override;

private:
  vtkGeoMath(const vtkGeoMath&) = delete;
  void operator=(const vtkGeoMath&) = delete;
};

#endif
