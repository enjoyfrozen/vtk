/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTDxMotionEventInfo
 * @brief   Store motion information from a 3DConnexion input device
 *
 * vtkTDxMotionEventInfo is a data structure that stores the information about
 * a motion event from a 3DConnexion input device.
 *
 * @sa
 * vtkTDxDevice
*/

#ifndef vtkTDxMotionEventInfo_h
#define vtkTDxMotionEventInfo_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkObject.h" // for the export macro

class VTKRENDERINGCORE_EXPORT vtkTDxMotionEventInfo
{
public:
  //@{
  /**
   * Translation coordinates
   */
  double X;
  double Y;
  double Z;
  //@}

  /**
   * Rotation angle.
   * The angle is in arbitrary unit.
   * It makes sense to have arbitrary unit
   * because the data comes from a device
   * where the information can be scaled by
   * the end-user.
   */
  double Angle;

  //@{
  /**
   * Rotation axis expressed as a unit vector.
   */
  double AxisX;
  double AxisY;
  double AxisZ;
};
  //@}

#endif
// VTK-HeaderTest-Exclude: vtkTDxMotionEventInfo.h
