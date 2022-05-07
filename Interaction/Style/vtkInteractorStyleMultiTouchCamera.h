/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleMultiTouchCamera.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
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
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderWindowInteractor.h" // for max pointers

class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyleMultiTouchCamera
  : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkInteractorStyleMultiTouchCamera* New();
  vtkTypeMacro(vtkInteractorStyleMultiTouchCamera, vtkInteractorStyleTrackballCamera);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Event bindings for gestures
   */
  void StartGesture() override;
  void OnStartRotate() override;
  void OnRotate() override;
  void OnEndRotate() override;
  void OnStartPinch() override;
  void OnPinch() override;
  void OnEndPinch() override;
  void OnStartPan() override;
  void OnPan() override;
  void OnEndPan() override;
  ///@}

  ///@{
  /**
   * Flag that enables/disables the recognition of touch gestures during rotation.
   * The flag may be enabled on systems where multi-touch gesture recognition is not robust
   * and a mouse event may be triggered right before a multi-touch gesture is detected. Since
   * transition from rotation to gesture may not be smooth (for example, there may be a jump
   * from touch position to gesture position) and after the multi-touch gesture the interrupted
   * rotation state is not restored, it is generally recommended to keep this flag disabled.
   * Disabled by default.
   */
  vtkSetMacro(AllowGesturesDuringRotate, bool);
  vtkGetMacro(AllowGesturesDuringRotate, bool);
  vtkBooleanMacro(AllowGesturesDuringRotate, bool);
  ///@}

protected:
  vtkInteractorStyleMultiTouchCamera();
  ~vtkInteractorStyleMultiTouchCamera() override;

  bool AllowGesturesDuringRotate;

private:
  vtkInteractorStyleMultiTouchCamera(const vtkInteractorStyleMultiTouchCamera&) = delete;
  void operator=(const vtkInteractorStyleMultiTouchCamera&) = delete;
};

#endif
