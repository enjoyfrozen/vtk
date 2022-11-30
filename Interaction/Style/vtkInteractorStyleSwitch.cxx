/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleSwitch.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInteractorStyleSwitch.h"

#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleJoystickActor.h"
#include "vtkInteractorStyleJoystickCamera.h"
#include "vtkInteractorStyleMultiTouchCamera.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"

VTK_ABI_NAMESPACE_BEGIN
namespace
{
enum StyleID
{
  JOYSTICK_CAMERA,
  JOYSTICK_ACTOR,
  TRACKBALL_CAMERA,
  TRACKBALL_ACTOR,
  MULTITOUCH_CAMERA
};
}

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkInteractorStyleSwitch);

//------------------------------------------------------------------------------
vtkInteractorStyleSwitch::vtkInteractorStyleSwitch()
{
  vtkNew<vtkInteractorStyleJoystickCamera> joystickCamera;
  vtkNew<vtkInteractorStyleJoystickActor> joystickActor;
  vtkNew<vtkInteractorStyleTrackballCamera> trackballCamera;
  vtkNew<vtkInteractorStyleTrackballActor> trackballActor;
  vtkNew<vtkInteractorStyleMultiTouchCamera> multiTouchCamera;

  this->AddStyle(joystickCamera);
  this->AddStyle(joystickActor);
  this->AddStyle(trackballCamera);
  this->AddStyle(trackballActor);
  this->AddStyle(multiTouchCamera);
}

//------------------------------------------------------------------------------
vtkInteractorStyleSwitch::~vtkInteractorStyleSwitch() = default;

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetInteractor(vtkRenderWindowInteractor* iren)
{
  if (iren == this->Interactor)
  {
    return;
  }

  // If we already have an interactor, stop observing it
  if (this->Interactor)
  {
    this->Interactor->RemoveObserver(this->EventCallbackCommand);
  }

  // Set the interactor to the internal styles.
  this->Superclass::SetInteractor(iren);

  // Add observer (i.e. ProcessEvents) for char and delete events.
  if (iren)
  {
    iren->AddObserver(vtkCommand::CharEvent, this->EventCallbackCommand, this->Priority);
    iren->AddObserver(vtkCommand::DeleteEvent, this->EventCallbackCommand, this->Priority);
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToJoystickActor()
{
  this->SetCurrentStyle(JOYSTICK_ACTOR);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToJoystickCamera()
{
  this->SetCurrentStyle(JOYSTICK_CAMERA);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToTrackballActor()
{
  this->SetCurrentStyle(TRACKBALL_ACTOR);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToTrackballCamera()
{
  this->SetCurrentStyle(TRACKBALL_CAMERA);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::SetCurrentStyleToMultiTouchCamera()
{
  this->SetCurrentStyle(MULTITOUCH_CAMERA);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::OnChar()
{
  switch (this->Interactor->GetKeyCode())
  {
    case 'j':
    case 'J':
      this->JoystickOrTrackball = VTKIS_JOYSTICK;
      this->EventCallbackCommand->SetAbortFlag(1);
      this->SetCurrentStyle((this->JoystickOrTrackball << 1) | this->CameraOrActor);
      break;
    case 't':
    case 'T':
      this->JoystickOrTrackball = VTKIS_TRACKBALL;
      this->EventCallbackCommand->SetAbortFlag(1);
      this->SetCurrentStyle((this->JoystickOrTrackball << 1) | this->CameraOrActor);
      break;
    case 'c':
    case 'C':
      this->CameraOrActor = VTKIS_CAMERA;
      this->EventCallbackCommand->SetAbortFlag(1);
      this->SetCurrentStyle((this->JoystickOrTrackball << 1) | this->CameraOrActor);
      break;
    case 'a':
    case 'A':
      this->CameraOrActor = VTKIS_ACTOR;
      this->EventCallbackCommand->SetAbortFlag(1);
      this->SetCurrentStyle((this->JoystickOrTrackball << 1) | this->CameraOrActor);
      break;
    case 'm':
    case 'M':
      this->EventCallbackCommand->SetAbortFlag(1);
      this->SetCurrentStyleToMultiTouchCamera();
      break;
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleSwitch::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END
