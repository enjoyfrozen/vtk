/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTDxInteractorStyleCamera
 * @brief   interactive manipulation of the camera with a 3DConnexion device
 *
 *
 * vtkTDxInteractorStyleCamera allows the end-user to manipulate tha camera
 * with a 3DConnexion device.
 *
 * @sa
 * vtkInteractorStyle vtkRenderWindowInteractor
 * vtkTDxInteractorStyle
*/

#ifndef vtkTDxInteractorStyleCamera_h
#define vtkTDxInteractorStyleCamera_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkTDxInteractorStyle.h"

class vtkTransform;

class VTKRENDERINGCORE_EXPORT vtkTDxInteractorStyleCamera : public vtkTDxInteractorStyle
{
public:
  static vtkTDxInteractorStyleCamera *New();
  vtkTypeMacro(vtkTDxInteractorStyleCamera,vtkTDxInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Action on motion event.
   * \pre: motionInfo_exist: motionInfo!=0
   */
  void OnMotionEvent(vtkTDxMotionEventInfo *motionInfo) override;

protected:
  vtkTDxInteractorStyleCamera();
  ~vtkTDxInteractorStyleCamera() override;

  vtkTransform *Transform; // Used for internal intermediate calculation.

private:
  vtkTDxInteractorStyleCamera(const vtkTDxInteractorStyleCamera&) = delete;
  void operator=(const vtkTDxInteractorStyleCamera&) = delete;
};
#endif
