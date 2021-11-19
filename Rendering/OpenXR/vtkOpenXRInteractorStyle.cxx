/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkOpenXRInteractorStyle.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOpenXRInteractorStyle.h"
#include "vtkCallbackCommand.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkOpenXRControlsHelper.h"
#include "vtkOpenXRRenderWindowInteractor.h"


// Map controller inputs to interaction states
vtkStandardNewMacro(vtkOpenXRInteractorStyle);

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::SetupActions(vtkRenderWindowInteractor* iren)
{
  vtkOpenXRRenderWindowInteractor* oiren = vtkOpenXRRenderWindowInteractor::SafeDownCast(iren);

  if (oiren)
  {
    oiren->AddAction("startmovement", vtkCommand::ViewerMovement3DEvent);
    oiren->AddAction("movement", vtkCommand::ViewerMovement3DEvent);
    oiren->AddAction("nextcamerapose", vtkCommand::NextPose3DEvent);
    oiren->AddAction("triggeraction", vtkCommand::Select3DEvent);
    oiren->AddAction("positionprop", vtkCommand::PositionProp3DEvent);
    oiren->AddAction("showmenu", vtkCommand::Menu3DEvent);
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::LoadNextCameraPose()
{
  vtkWarningMacro("Not implemented yet");
}

//------------------------------------------------------------------------------
vtkVRControlsHelper* vtkOpenXRInteractorStyle::MakeControlsHelper()
{
  vtkVRControlsHelper* helper = vtkOpenXRControlsHelper::New();
  return helper;
}

