/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkInteractorStyleMultiTouchCamera
 * @brief   multitouch manipulation of the camera
 *
 * vtkInteractorStyleMultiTouchCamera allows the user to interactively
 * manipulate (rotate, pan, etc.) the camera, the viewpoint of the scene
 * using multitouch gestures in addition to regular gestures
 *
 * @sa
 * vtkInteractorStyleTrackballActor vtkInteractorStyleJoystickCamera
 * vtkInteractorStyleJoystickActor
*/

#ifndef vtkInteractorStyleMultiTouchCamera_h
#define vtkInteractorStyleMultiTouchCamera_h

#include "vtkInteractionStyleModule.h" // For export macro
#include "vtkRenderWindowInteractor.h" // for max pointers
#include "vtkInteractorStyleTrackballCamera.h"

class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyleMultiTouchCamera : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkInteractorStyleMultiTouchCamera *New();
  vtkTypeMacro(vtkInteractorStyleMultiTouchCamera,vtkInteractorStyleTrackballCamera);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Event bindings for gestures
   */
  void OnStartRotate() override;
  void OnRotate() override;
  void OnEndRotate() override;
  void OnStartPinch() override;
  void OnPinch() override;
  void OnEndPinch() override;
  void OnStartPan() override;
  void OnPan() override;
  void OnEndPan() override;

  //@}

protected:
  vtkInteractorStyleMultiTouchCamera();
  ~vtkInteractorStyleMultiTouchCamera() override;

private:
  vtkInteractorStyleMultiTouchCamera(const vtkInteractorStyleMultiTouchCamera&) = delete;
  void operator=(const vtkInteractorStyleMultiTouchCamera&) = delete;
};

#endif
