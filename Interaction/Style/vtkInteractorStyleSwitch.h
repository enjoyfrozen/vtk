/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleSwitch.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkInteractorStyleSwitch
 * @brief   class to swap between interactory styles
 *
 * The class vtkInteractorStyleSwitch allows handles interactively switching
 * between five interactor styles: joystick actor, joystick camera,
 * trackball actor, trackball camera and multitouch camera.
 * Type 'j' or 't' to select joystick or trackball,
 * and type 'c' or 'a' to select camera or actor.
 * The default interactor style is joystick camera.
 * @sa
 * vtkInteractorStyleJoystickActor vtkInteractorStyleJoystickCamera
 * vtkInteractorStyleTrackballActor vtkInteractorStyleTrackballCamera
 */

#ifndef vtkInteractorStyleSwitch_h
#define vtkInteractorStyleSwitch_h

#include "vtkInteractionStyleModule.h" // For export macro
#include "vtkInteractorStyleSwitchBase.h"

#define VTKIS_JOYSTICK 0
#define VTKIS_TRACKBALL 1

#define VTKIS_CAMERA 0
#define VTKIS_ACTOR 1

VTK_ABI_NAMESPACE_BEGIN
class vtkInteractorStyleJoystickActor;
class vtkInteractorStyleJoystickCamera;
class vtkInteractorStyleTrackballActor;
class vtkInteractorStyleTrackballCamera;
class vtkInteractorStyleMultiTouchCamera;

class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyleSwitch : public vtkInteractorStyleSwitchBase
{
public:
  static vtkInteractorStyleSwitch* New();
  vtkTypeMacro(vtkInteractorStyleSwitch, vtkInteractorStyleSwitchBase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Overriden to add the key observer allwing to change
   * the internal interactor style by pressing the
   * appropriate keys.
   */
  void SetInteractor(vtkRenderWindowInteractor* iren) override;

  ///@{
  /**
   * Convenience methods to Set/Get the current internal
   * interactor style.
   */
  void SetCurrentStyleToJoystickActor();
  void SetCurrentStyleToJoystickCamera();
  void SetCurrentStyleToTrackballActor();
  void SetCurrentStyleToTrackballCamera();
  void SetCurrentStyleToMultiTouchCamera();
  ///@}

  /**
   * In this interactor style, we only care about the char events,
   * which are used to switch between different interactor styles.
   */
  void OnChar() override;

protected:
  vtkInteractorStyleSwitch();
  ~vtkInteractorStyleSwitch() override;

  /**
   * Internal method used to choose the internal style depending
   * on the keys combination.
   */
  void SetCurrentStyleInternal();

private:
  vtkInteractorStyleSwitch(const vtkInteractorStyleSwitch&) = delete;
  void operator=(const vtkInteractorStyleSwitch&) = delete;

  int JoystickOrTrackball = VTKIS_JOYSTICK;
  int CameraOrActor = VTKIS_CAMERA;
};

VTK_ABI_NAMESPACE_END
#endif
