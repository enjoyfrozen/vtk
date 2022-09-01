/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleTrackballCamera.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkInteractorStyleTrackballCamera
 * @brief   interactive manipulation of the camera
 *
 * vtkInteractorStyleTrackballCamera allows the user to interactively
 * manipulate (rotate, pan, etc.) the camera, the viewpoint of the scene.  In
 * trackball interaction, the magnitude of the mouse motion is proportional
 * to the camera motion associated with a particular mouse binding. For
 * example, small left-button motions cause small changes in the rotation of
 * the camera around its focal point. For a 3-button mouse, the left button
 * is for rotation, the right button for zooming, the middle button for
 * panning, ctrl + left button for spinning, and shift + right button for
 * environment rotation. (With fewer mouse buttons, ctrl + shift + left button
 * is for zooming, and shift + left button is for panning.)
 *
 * @sa
 * vtkInteractorStyleTrackballActor vtkInteractorStyleJoystickCamera
 * vtkInteractorStyleJoystickActor
 */

#ifndef vtkInteractorStyleTrackballCamera_h
#define vtkInteractorStyleTrackballCamera_h

#include "vtkInteractionStyleModule.h" // For export macro
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleCameraUtils.h"

VTK_ABI_NAMESPACE_BEGIN
class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyleTrackballCamera : public vtkInteractorStyle
{
public:
  static vtkInteractorStyleTrackballCamera* New();
  vtkTypeMacro(vtkInteractorStyleTrackballCamera, vtkInteractorStyle);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Event bindings controlling the effects of pressing mouse buttons
   * or moving the mouse.
   */
  void OnMouseMove() override;
  void OnLeftButtonDown() override;
  void OnLeftButtonUp() override;
  virtual void OnLeftButtonSingleClick();
  void OnMiddleButtonDown() override;
  void OnMiddleButtonUp() override;
  virtual void OnMiddleButtonSingleClick();
  void OnRightButtonDown() override;
  void OnRightButtonUp() override;
  virtual void OnRightButtonSingleClick();
  void OnMouseWheelForward() override;
  void OnMouseWheelBackward() override;
  ///@}

  // These methods for the different interactions in different modes
  // are overridden in subclasses to perform the correct motion. Since
  // they are called by OnTimer, they do not have mouse coord parameters
  // (use interactor's GetEventPosition and GetLastEventPosition)
  void Rotate() override;
  virtual bool CanRepeatRotation();
  virtual void RepeatRotation();
  void Spin() override;
  void Pan() override;
  void Dolly() override;
  void EnvironmentRotate() override;

  ///@{
  /**
   * Sets a radius the mouse must stay within to detect a click.
   *
   * When people click a mouse button, it is very common for the mouse click action to minutely move
   * the mouse. This can make it very difficult to differentiate ButtonClick from Rotation because
   * ButtonUp and Down will naturally occur at different pixel locations. By setting a larger
   * radius, we allow for a small rotation to still be dectected as a click. This value should be
   * tuned large enough to make clicking easy, but not so large the small rotations are still
   * recognized as clicks.
   *
   * Default: 1.5 (enough to cover a 1 pixel radius around the clicked pixel, including on
   * diagonals)
   */
  vtkSetMacro(ClickTolerance, double);
  vtkGetMacro(ClickTolerance, double);
  ///@}

  ///@{
  /**
   * Which dolly model should be used to map user interaction into a camera dolly.
   * Default: vtkDollyModel::Default
   */
  vtkSetEnumMacro(DollyModel, vtkDollyModel);
  vtkGetEnumMacro(DollyModel, vtkDollyModel);
  ///@}

  ///@{
  /**
   * Set the apparent sensitivity of the interactor style to mouse motion.
   */
  vtkSetMacro(MotionFactor, double);
  vtkGetMacro(MotionFactor, double);
  ///@}

  ///@{
  /**
   * Set the apparent sensitivity of the interactor style to mouse motion.
   */
  vtkSetMacro(MotionFactorSingularityRotation, double);
  vtkGetMacro(MotionFactorSingularityRotation, double);
  ///@}

  ///@{
  /**
   * Invert the direction of mouse wheel movement. This switches from camera-centric to
   * model-centric scroll wheel movement.
   */
  vtkSetMacro(MouseWheelInvertDirection, bool);
  vtkGetMacro(MouseWheelInvertDirection, bool);
  ///@}

  ///@{
  /**
   * Sets whether rotation operations can happen or if pan operations should default.
   */
  vtkSetMacro(RotationEnabled, bool);
  vtkGetMacro(RotationEnabled, bool);
  ///@}

  ///@{
  /**
   * Which rotation model should be used to map user interaction into a rotation.
   * Default: vtkTrackballRotationModel::Default
   */
  vtkSetEnumMacro(RotationModel, vtkTrackballRotationModel);
  vtkGetEnumMacro(RotationModel, vtkTrackballRotationModel);
  ///@}

protected:
  vtkInteractorStyleTrackballCamera();
  ~vtkInteractorStyleTrackballCamera() override;

  double ClickTolerance = 1.5;
  vtkDollyModel DollyModel = vtkDollyModel::Default;
  double MotionFactor = 10.0;
  double MotionFactorSingularityRotation = 10.0;
  bool MouseWheelInvertDirection = false;
  bool RotationEnabled = true;
  vtkTrackballRotationModel RotationModel = vtkTrackballRotationModel::Default;

  ///@{
  /**
   * Utility methods for recording button event positions.
   */
  vtkGetVector2Macro(LeftButtonDownPosition, int);
  vtkSetVector2Macro(LeftButtonDownPosition, int);
  vtkGetVector2Macro(MiddleButtonDownPosition, int);
  vtkSetVector2Macro(MiddleButtonDownPosition, int);
  vtkGetVector2Macro(RightButtonDownPosition, int);
  vtkSetVector2Macro(RightButtonDownPosition, int);
  ///@}
  int LeftButtonDownPosition[2] = { 0, 0 };
  int MiddleButtonDownPosition[2] = { 0, 0 };
  int RightButtonDownPosition[2] = { 0, 0 };

  double SingularityRotationAxis[3] = { 0.0, 0.0, 0.0 };
  double SingularityRotationAngle = 0.0;

  double ConstrainedRotationPhi = 0.0;
  double ConstrainedRotationTheta = 0.0;

  virtual void OnMouseWheelAction(double direction);

  virtual void RotateDefault();
  ///@{
  /**
   * Rotates the camera around its focal point according to the current values for the rotation axis
   * and angle.
   */
  virtual void RotateSingularity();
  ///@}
  virtual void RotateSingularityCalculateAxisAndAngle();
  virtual void RotateWorldZScreenX();

  virtual void Dolly(double factor);

private:
  vtkInteractorStyleTrackballCamera(const vtkInteractorStyleTrackballCamera&) = delete;
  void operator=(const vtkInteractorStyleTrackballCamera&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif
