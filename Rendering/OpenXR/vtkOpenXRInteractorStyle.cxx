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

#include "vtkCompositeDataSet.h"
#include "vtkDataObjectTreeIterator.h"

#include "vtkBillboardTextActor3D.h"
#include "vtkCoordinate.h"
#include "vtkTextActor.h"
#include "vtkTextActor3D.h"

#include "vtkAssemblyPath.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkCellPicker.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkHardwareSelector.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkMapper.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPropPicker.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSphereSource.h"
#include "vtkStringArray.h"
#include "vtkTextProperty.h"
#include "vtkTimerLog.h"
#include "vtkVRHardwarePicker.h"

#include "vtkOpenXRCamera.h"
#include "vtkOpenXRRenderWindow.h"
#include "vtkOpenXRRenderWindowInteractor.h"
#include "vtkVRMenuRepresentation.h"
#include "vtkVRMenuWidget.h"
#include "vtkVRModel.h"

#include "vtkWidgetRepresentation.h"

#include <sstream>

// Map controller inputs to interaction states
vtkStandardNewMacro(vtkOpenXRInteractorStyle);

//------------------------------------------------------------------------------
vtkOpenXRInteractorStyle::vtkOpenXRInteractorStyle()
{
  for (int d = 0; d < vtkEventDataNumberOfDevices; ++d)
  {
    this->InteractionState[d] = VTKIS_NONE;
    this->InteractionProps[d] = nullptr;
    this->ClippingPlanes[d] = nullptr;
  }

  // Create default inputs mapping
  this->MapInputToAction(vtkCommand::ViewerMovement3DEvent, VTKIS_DOLLY);
  this->MapInputToAction(vtkCommand::Menu3DEvent, VTKIS_MENU);
  this->MapInputToAction(vtkCommand::NextPose3DEvent, VTKIS_LOAD_CAMERA_POSE);
  this->MapInputToAction(vtkCommand::Select3DEvent, VTKIS_POSITION_PROP);

  this->MenuCommand->SetClientData(this);
  this->MenuCommand->SetCallback(vtkOpenXRInteractorStyle::MenuCallback);

  this->Menu->SetRepresentation(this->MenuRepresentation);
  this->Menu->PushFrontMenuItem("exit", "Exit", this->MenuCommand);
  this->Menu->PushFrontMenuItem("clipmode", "Clipping Mode", this->MenuCommand);
  this->Menu->PushFrontMenuItem("probemode", "Probe Mode", this->MenuCommand);
  this->Menu->PushFrontMenuItem("grabmode", "Grab Mode", this->MenuCommand);

  vtkNew<vtkPolyDataMapper> pdm;
  this->PickActor->SetMapper(pdm);
  this->PickActor->GetProperty()->SetLineWidth(4);
  this->PickActor->GetProperty()->RenderLinesAsTubesOn();
  this->PickActor->GetProperty()->SetRepresentationToWireframe();
  this->PickActor->DragableOff();

  vtkNew<vtkCellPicker> exactPicker;
  this->SetInteractionPicker(exactPicker);
}

//------------------------------------------------------------------------------
vtkOpenXRInteractorStyle::~vtkOpenXRInteractorStyle()
{
  for (int d = 0; d < vtkEventDataNumberOfDevices; ++d)
  {
    if (this->ClippingPlanes[d])
    {
      this->ClippingPlanes[d]->Delete();
    }
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::SetInteractor(vtkRenderWindowInteractor* iren)
{
  this->Superclass::SetInteractor(iren);

  // hook up default bindings?
  auto oiren = vtkOpenXRRenderWindowInteractor::SafeDownCast(iren);
  if (!oiren)
  {
    return;
  }

  oiren->AddAction("startmovement", vtkCommand::ViewerMovement3DEvent);
  oiren->AddAction("movement", vtkCommand::ViewerMovement3DEvent);
  oiren->AddAction("nextcamerapose", vtkCommand::NextPose3DEvent);
  oiren->AddAction("triggeraction", vtkCommand::Select3DEvent);
  oiren->AddAction("positionprop", vtkCommand::PositionProp3DEvent);
  oiren->AddAction("showmenu", vtkCommand::Menu3DEvent);
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "HoverPick: " << this->HoverPick;
  os << indent << "GrabWithRay: " << this->GrabWithRay;
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::MenuCallback(
  vtkObject* vtkNotUsed(object), unsigned long, void* clientdata, void* calldata)
{
  std::string name = static_cast<const char*>(calldata);
  vtkOpenXRInteractorStyle* self = static_cast<vtkOpenXRInteractorStyle*>(clientdata);

  if (name == "exit")
  {
    if (self->Interactor)
    {
      self->Interactor->ExitCallback();
    }
  }
  if (name == "togglelabel")
  {
    self->ToggleDrawControls();
  }
  if (name == "clipmode")
  {
    self->MapInputToAction(vtkCommand::Select3DEvent, VTKIS_CLIP);
  }
  if (name == "grabmode")
  {
    self->MapInputToAction(vtkCommand::Select3DEvent, VTKIS_POSITION_PROP);
  }
  if (name == "probemode")
  {
    self->MapInputToAction(vtkCommand::Select3DEvent, VTKIS_PICK);
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnNextPose3D(vtkEventData* edata)
{
  vtkEventDataDevice3D* edd = edata->GetAsEventDataDevice3D();
  if (!edd)
  {
    return;
  }
  if (edd->GetAction() == vtkEventDataAction::Press)
  {
    this->LoadNextCameraPose();
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnViewerMovement3D(vtkEventData* edata)
{
  vtkEventDataDevice3D* edd = edata->GetAsEventDataDevice3D();
  if (!edd)
  {
    return;
  }

  // joystick moves?
  int idev = static_cast<int>(edd->GetDevice());

  // Update current state
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];
  this->FindPokedRenderer(x, y);

  // Set current state and interaction prop
  this->InteractionProp = this->InteractionProps[idev];

  double const* pos = edd->GetTrackPadPosition();

  if (edd->GetAction() == vtkEventDataAction::Press)
  {
    this->StartAction(VTKIS_DOLLY, edd);
    this->LastTrackPadPosition[0] = 0.0;
    this->LastTrackPadPosition[1] = 0.0;
    return;
  }

  if (edd->GetAction() == vtkEventDataAction::Release)
  {
    this->EndAction(VTKIS_DOLLY, edd);
    return;
  }

  // if the event is joystick and it is away from the center then
  // call start, when returning to center call end
  if (edd->GetInput() == vtkEventDataDeviceInput::Joystick &&
    this->InteractionState[idev] != VTKIS_DOLLY && pos[1] != 0.0)
  {
    this->StartAction(VTKIS_DOLLY, edd);
    this->LastTrackPadPosition[0] = 0.0;
    this->LastTrackPadPosition[1] = 0.0;
    return;
  }

  if (this->InteractionState[idev] == VTKIS_DOLLY)
  {
    // strop when returning to zero on joystick
    if (edd->GetInput() == vtkEventDataDeviceInput::Joystick && pos[1] == 0.0)
    {
      this->EndAction(VTKIS_DOLLY, edd);
      return;
    }
    this->Dolly3D(edata);
    this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
  }
}

//------------------------------------------------------------------------------
// Generic events binding
//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnMove3D(vtkEventData* edata)
{
  vtkEventDataDevice3D* edd = edata->GetAsEventDataDevice3D();
  if (!edd)
  {
    return;
  }

  // joystick moves?
  int idev = static_cast<int>(edd->GetDevice());

  // Update current state
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);

  // Set current state and interaction prop
  this->InteractionProp = this->InteractionProps[idev];

  switch (this->InteractionState[idev])
  {
    case VTKIS_POSITION_PROP:
      this->PositionProp(edd);
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;
    case VTKIS_DOLLY:
      // this->Dolly3D(edata);
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;
    case VTKIS_CLIP:
      this->Clip(edd);
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;
  }

  // Update rays
  this->UpdateRay(edd->GetDevice());
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnMenu3D(vtkEventData* edata)
{
  vtkEventDataDevice3D* edd = edata->GetAsEventDataDevice3D();
  if (!edd)
  {
    return;
  }

  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];
  this->FindPokedRenderer(x, y);

  if (edd->GetAction() == vtkEventDataAction::Press)
  {
    this->StartAction(VTKIS_MENU, edd);
    return;
  }

  if (edd->GetAction() == vtkEventDataAction::Release)
  {
    this->EndAction(VTKIS_MENU, edd);
    return;
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnSelect3D(vtkEventData* edata)
{
  vtkEventDataDevice3D* bd = edata->GetAsEventDataDevice3D();
  if (!bd)
  {
    return;
  }

  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];
  this->FindPokedRenderer(x, y);

  decltype(this->InputMap)::key_type key(vtkCommand::Select3DEvent, bd->GetAction());
  auto it = this->InputMap.find(key);
  if (it == this->InputMap.end())
  {
    return;
  }

  int state = it->second;

  // if grab mode then convert event data into where the ray is intersecting geometry

  if (bd->GetAction() == vtkEventDataAction::Press)
  {
    this->StartAction(state, bd);
  }
  if (bd->GetAction() == vtkEventDataAction::Release)
  {
    this->EndAction(state, bd);
  }
  if (bd->GetAction() == vtkEventDataAction::Touch)
  {
    this->StartAction(state, bd);
  }
  if (bd->GetAction() == vtkEventDataAction::Untouch)
  {
    this->EndAction(state, bd);
  }
}

//------------------------------------------------------------------------------
// Interaction entry points
//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::StartPick(vtkEventDataDevice3D* edata)
{
  this->HideBillboard();
  this->HidePickActor();

  this->InteractionState[static_cast<int>(edata->GetDevice())] = VTKIS_PICK;

  // update ray
  this->UpdateRay(edata->GetDevice());
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::EndPick(vtkEventDataDevice3D* edata)
{
  // perform probe
  this->ProbeData(edata->GetDevice());

  this->InteractionState[static_cast<int>(edata->GetDevice())] = VTKIS_NONE;

  // turn off ray
  this->UpdateRay(edata->GetDevice());
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::StartLoadCamPose(vtkEventDataDevice3D* edata)
{
  int iDevice = static_cast<int>(edata->GetDevice());
  this->InteractionState[iDevice] = VTKIS_LOAD_CAMERA_POSE;
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::EndLoadCamPose(vtkEventDataDevice3D* edata)
{
  this->LoadNextCameraPose();

  int iDevice = static_cast<int>(edata->GetDevice());
  this->InteractionState[iDevice] = VTKIS_NONE;
}

//------------------------------------------------------------------------------
bool vtkOpenXRInteractorStyle::HardwareSelect(vtkEventDataDevice controller, bool actorPassOnly)
{
  vtkRenderer* ren = this->CurrentRenderer;
  vtkOpenXRRenderWindow* renWin =
    vtkOpenXRRenderWindow::SafeDownCast(this->Interactor->GetRenderWindow());
  vtkOpenXRRenderWindowInteractor* iren =
    static_cast<vtkOpenXRRenderWindowInteractor*>(this->Interactor);

  if (!ren || !renWin || !iren)
  {
    return false;
  }

  this->HideRay(controller);

  // Compute controller position and world orientation
  const int trackedDeviceIndex = renWin->GetTrackedDeviceIndexForDevice(controller);
  if (trackedDeviceIndex == vtkOpenXRManager::ControllerIndex::Inactive)
  {
    return false;
  }
  double p0[3];   // Ray start point
  double wxyz[4]; // Controller orientation
  double dummy_ppos[3];
  double wdir[3];
  XrPosef tdPose = iren->GetHandPose(trackedDeviceIndex);
  iren->ConvertOpenXRPoseToWorldCoordinates(tdPose, p0, wxyz, dummy_ppos, wdir);

  this->HardwarePicker->PickProp(p0, wxyz, ren, ren->GetViewProps(), actorPassOnly);

  this->ShowRay(controller);

  return true;
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::StartPositionProp(vtkEventDataDevice3D* edata)
{
  if (this->GrabWithRay)
  {
    if (!this->HardwareSelect(edata->GetDevice(), true))
    {
      return;
    }

    vtkSelection* selection = this->HardwarePicker->GetSelection();

    if (!selection || selection->GetNumberOfNodes() == 0)
    {
      return;
    }

    vtkSelectionNode* node = selection->GetNode(0);
    this->InteractionProp =
      vtkProp3D::SafeDownCast(node->GetProperties()->Get(vtkSelectionNode::PROP()));
  }
  else
  {
    double pos[3];
    edata->GetWorldPosition(pos);
    this->FindPickedActor(pos, nullptr);
  }

  if (this->InteractionProp == nullptr)
  {
    return;
  }

  this->InteractionState[static_cast<int>(edata->GetDevice())] = VTKIS_POSITION_PROP;
  this->InteractionProps[static_cast<int>(edata->GetDevice())] = this->InteractionProp;

  // Don't start action if a controller is already positioning the prop
  int rc = static_cast<int>(vtkEventDataDevice::RightController);
  int lc = static_cast<int>(vtkEventDataDevice::LeftController);
  if (this->InteractionProps[rc] == this->InteractionProps[lc])
  {
    this->EndPositionProp(edata);
    return;
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::EndPositionProp(vtkEventDataDevice3D* edata)
{
  vtkEventDataDevice dev = edata->GetDevice();
  this->InteractionState[static_cast<int>(dev)] = VTKIS_NONE;
  this->InteractionProps[static_cast<int>(dev)] = nullptr;
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::StartClip(vtkEventDataDevice3D* ed)
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkEventDataDevice dev = ed->GetDevice();
  this->InteractionState[static_cast<int>(dev)] = VTKIS_CLIP;

  if (!this->ClippingPlanes[static_cast<int>(dev)])
  {
    this->ClippingPlanes[static_cast<int>(dev)] = vtkPlane::New();
  }

  vtkActorCollection* ac;
  vtkActor *anActor, *aPart;
  vtkAssemblyPath* path;
  if (this->CurrentRenderer != 0)
  {
    ac = this->CurrentRenderer->GetActors();
    vtkCollectionSimpleIterator ait;
    for (ac->InitTraversal(ait); (anActor = ac->GetNextActor(ait));)
    {
      for (anActor->InitPathTraversal(); (path = anActor->GetNextPath());)
      {
        aPart = static_cast<vtkActor*>(path->GetLastNode()->GetViewProp());
        if (aPart->GetMapper())
        {
          aPart->GetMapper()->AddClippingPlane(this->ClippingPlanes[static_cast<int>(dev)]);
          continue;
        }
      }
    }
  }
  else
  {
    vtkWarningMacro(<< "no current renderer on the interactor style.");
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::EndClip(vtkEventDataDevice3D* ed)
{
  vtkEventDataDevice dev = ed->GetDevice();
  this->InteractionState[static_cast<int>(dev)] = VTKIS_NONE;

  vtkActorCollection* ac;
  vtkActor *anActor, *aPart;
  vtkAssemblyPath* path;
  if (this->CurrentRenderer != 0)
  {
    ac = this->CurrentRenderer->GetActors();
    vtkCollectionSimpleIterator ait;
    for (ac->InitTraversal(ait); (anActor = ac->GetNextActor(ait));)
    {
      for (anActor->InitPathTraversal(); (path = anActor->GetNextPath());)
      {
        aPart = static_cast<vtkActor*>(path->GetLastNode()->GetViewProp());
        if (aPart->GetMapper())
        {
          aPart->GetMapper()->RemoveClippingPlane(this->ClippingPlanes[static_cast<int>(dev)]);
          continue;
        }
      }
    }
  }
  else
  {
    vtkWarningMacro(<< "no current renderer on the interactor style.");
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::StartDolly3D(vtkEventDataDevice3D* ed)
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }
  vtkEventDataDevice dev = ed->GetDevice();
  this->InteractionState[static_cast<int>(dev)] = VTKIS_DOLLY;
  this->LastDolly3DEventTime->StartTimer();
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::EndDolly3D(vtkEventDataDevice3D* ed)
{
  vtkEventDataDevice dev = ed->GetDevice();
  this->InteractionState[static_cast<int>(dev)] = VTKIS_NONE;

  this->LastDolly3DEventTime->StopTimer();
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::ToggleDrawControls()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::SetDrawControls(bool vtkNotUsed(val))
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }
}

//------------------------------------------------------------------------------
// Interaction methods
//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::ProbeData(vtkEventDataDevice controller)
{
  // Invoke start pick method if defined
  this->InvokeEvent(vtkCommand::StartPickEvent, nullptr);

  if (!this->HardwareSelect(controller, false))
  {
    return;
  }

  // Invoke end pick method if defined
  if (this->HandleObservers && this->HasObserver(vtkCommand::EndPickEvent))
  {
    this->InvokeEvent(vtkCommand::EndPickEvent, this->HardwarePicker->GetSelection());
  }
  else
  {
    this->EndPickCallback(this->HardwarePicker->GetSelection());
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::EndPickCallback(vtkSelection* sel)
{
  if (!sel)
  {
    return;
  }

  vtkSelectionNode* node = sel->GetNode(0);
  if (!node || !node->GetProperties()->Has(vtkSelectionNode::PROP()))
  {
    return;
  }

  vtkProp3D* prop = vtkProp3D::SafeDownCast(node->GetProperties()->Get(vtkSelectionNode::PROP()));
  if (!prop)
  {
    return;
  }
  this->ShowPickSphere(prop->GetCenter(), prop->GetLength() / 2.0, nullptr);
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::LoadNextCameraPose()
{
  vtkOpenXRRenderWindow* renWin =
    vtkOpenXRRenderWindow::SafeDownCast(this->Interactor->GetRenderWindow());
  if (!renWin)
  {
    return;
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::PositionProp(vtkEventData* ed, double* lwpos, double* lwori)
{
  if (this->InteractionProp == nullptr || !this->InteractionProp->GetDragable())
  {
    return;
  }
  this->Superclass::PositionProp(ed, lwpos, lwori);
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::Clip(vtkEventDataDevice3D* ed)
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  const double* wpos = ed->GetWorldPosition();
  const double* wori = ed->GetWorldOrientation();

  double ori[4];
  ori[0] = vtkMath::RadiansFromDegrees(wori[0]);
  ori[1] = wori[1];
  ori[2] = wori[2];
  ori[3] = wori[3];

  // we have a position and a normal, that defines our plane
  double r[3];
  double up[3];
  up[0] = 0;
  up[1] = -1;
  up[2] = 0;
  vtkMath::RotateVectorByWXYZ(up, ori, r);

  vtkEventDataDevice dev = ed->GetDevice();
  int idev = static_cast<int>(dev);
  this->ClippingPlanes[idev]->SetNormal(r);
  this->ClippingPlanes[idev]->SetOrigin(wpos[0], wpos[1], wpos[2]);
}

//------------------------------------------------------------------------------
// Multitouch interaction methods
//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnPan()
{
  int rc = static_cast<int>(vtkEventDataDevice::RightController);
  int lc = static_cast<int>(vtkEventDataDevice::LeftController);

  if (!this->InteractionProps[rc] && !this->InteractionProps[lc])
  {
    this->InteractionState[rc] = VTKIS_PAN;
    this->InteractionState[lc] = VTKIS_PAN;

    int pointer = this->Interactor->GetPointerIndex();

    this->FindPokedRenderer(this->Interactor->GetEventPositions(pointer)[0],
      this->Interactor->GetEventPositions(pointer)[1]);

    if (this->CurrentRenderer == nullptr)
    {
      return;
    }

    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    vtkRenderWindowInteractor3D* rwi = static_cast<vtkRenderWindowInteractor3D*>(this->Interactor);

    double t[3] = { rwi->GetTranslation3D()[0] - rwi->GetLastTranslation3D()[0],
      rwi->GetTranslation3D()[1] - rwi->GetLastTranslation3D()[1],
      rwi->GetTranslation3D()[2] - rwi->GetLastTranslation3D()[2] };

    double* ptrans = rwi->GetPhysicalTranslation(camera);

    rwi->SetPhysicalTranslation(camera, ptrans[0] + t[0], ptrans[1] + t[1], ptrans[2] + t[2]);

    // clean up
    if (this->Interactor->GetLightFollowCamera())
    {
      this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnPinch()
{
  int rc = static_cast<int>(vtkEventDataDevice::RightController);
  int lc = static_cast<int>(vtkEventDataDevice::LeftController);

  if (!this->InteractionProps[rc] && !this->InteractionProps[lc])
  {
    this->InteractionState[rc] = VTKIS_ZOOM;
    this->InteractionState[lc] = VTKIS_ZOOM;

    int pointer = this->Interactor->GetPointerIndex();

    this->FindPokedRenderer(this->Interactor->GetEventPositions(pointer)[0],
      this->Interactor->GetEventPositions(pointer)[1]);

    if (this->CurrentRenderer == nullptr)
    {
      return;
    }

    double dyf = this->Interactor->GetScale() / this->Interactor->GetLastScale();
    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    vtkRenderWindowInteractor3D* rwi = static_cast<vtkRenderWindowInteractor3D*>(this->Interactor);
    double physicalScale = rwi->GetPhysicalScale();

    this->SetScale(camera, physicalScale / dyf);
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::OnRotate()
{
  int rc = static_cast<int>(vtkEventDataDevice::RightController);
  int lc = static_cast<int>(vtkEventDataDevice::LeftController);

  // Rotate only when one controller is not interacting
  if (!this->InteractionProps[rc] && !this->InteractionProps[lc])
  {
    this->InteractionState[rc] = VTKIS_ROTATE;
    this->InteractionState[lc] = VTKIS_ROTATE;

    double angle = this->Interactor->GetRotation() - this->Interactor->GetLastRotation();
    if (fabs(angle) > 90)
    {
      return;
    }

    // rotate the world, aka rotate the physicalViewDirection about the physicalViewUp
    vtkOpenXRRenderWindow* renWin =
      vtkOpenXRRenderWindow::SafeDownCast(this->Interactor->GetRenderWindow());
    if (!renWin)
    {
      return;
    }

    double* vup = renWin->GetPhysicalViewUp();
    double* dop = renWin->GetPhysicalViewDirection();
    double newDOP[3];
    double wxyz[4];
    wxyz[0] = vtkMath::RadiansFromDegrees(angle);
    wxyz[1] = vup[0];
    wxyz[2] = vup[1];
    wxyz[3] = vup[2];
    vtkMath::RotateVectorByWXYZ(dop, wxyz, newDOP);
    renWin->SetPhysicalViewDirection(newDOP);
  }
}

//------------------------------------------------------------------------------
// Utility routines
//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::MapInputToAction(
  vtkCommand::EventIds eid, vtkEventDataAction action, int state)
{
  if (state < VTKIS_NONE)
  {
    return;
  }

  decltype(this->InputMap)::key_type key(eid, action);
  auto it = this->InputMap.find(key);
  if (it != this->InputMap.end())
  {
    if (it->second == state)
    {
      return;
    }
  }

  this->InputMap[key] = state;

  this->Modified();
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::MapInputToAction(vtkCommand::EventIds eid, int state)
{
  this->MapInputToAction(eid, vtkEventDataAction::Press, state);
  this->MapInputToAction(eid, vtkEventDataAction::Release, state);
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::StartAction(int state, vtkEventDataDevice3D* edata)
{
  switch (state)
  {
    case VTKIS_POSITION_PROP:
      this->StartPositionProp(edata);
      break;
    case VTKIS_DOLLY:
      this->StartDolly3D(edata);
      break;
    case VTKIS_CLIP:
      this->StartClip(edata);
      break;
    case VTKIS_PICK:
      this->StartPick(edata);
      break;
    case VTKIS_LOAD_CAMERA_POSE:
      this->StartLoadCamPose(edata);
      break;
    default:
      vtkWarningMacro(<< "StartAction: unknown state: " << state);
      break;
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::EndAction(int state, vtkEventDataDevice3D* edata)
{
  switch (state)
  {
    case VTKIS_POSITION_PROP:
      this->EndPositionProp(edata);
      break;
    case VTKIS_DOLLY:
      this->EndDolly3D(edata);
      break;
    case VTKIS_CLIP:
      this->EndClip(edata);
      break;
    case VTKIS_PICK:
      this->EndPick(edata);
      break;
    case VTKIS_MENU:
      this->Menu->SetInteractor(this->Interactor);
      this->Menu->Show(edata);
      break;
    case VTKIS_LOAD_CAMERA_POSE:
      this->EndLoadCamPose(edata);
      break;
    case VTKIS_TOGGLE_DRAW_CONTROLS:
      this->ToggleDrawControls();
      break;
    case VTKIS_EXIT:
      if (this->Interactor)
      {
        this->Interactor->ExitCallback();
      }
      break;
    default:
      vtkWarningMacro(<< "EndAction: unknown state " << state);
      break;
  }

  // Reset multitouch state because a button has been released
  for (int d = 0; d < vtkEventDataNumberOfDevices; ++d)
  {
    switch (this->InteractionState[d])
    {
      case VTKIS_PAN:
      case VTKIS_ZOOM:
      case VTKIS_ROTATE:
        this->InteractionState[d] = VTKIS_NONE;
        break;
      default:
        vtkWarningMacro(<< "EndAction: unknown interaction state " << d << ": "
                        << this->InteractionState[d]);
        break;
    }
  }
}

//------------------------------------------------------------------------------
// Handle Ray drawing and update
//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::ShowRay(vtkEventDataDevice controller)
{
  vtkOpenXRRenderWindow* renWin =
    vtkOpenXRRenderWindow::SafeDownCast(this->Interactor->GetRenderWindow());
  if (!renWin ||
    (controller != vtkEventDataDevice::LeftController &&
      controller != vtkEventDataDevice::RightController))
  {
    return;
  }

  const int trackedDeviceIndex = renWin->GetTrackedDeviceIndexForDevice(controller);
  vtkVRModel* model = renWin->GetTrackedDeviceModel(trackedDeviceIndex);

  if (model)
  {
    model->SetShowRay(true);
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::HideRay(vtkEventDataDevice controller)
{
  vtkOpenXRRenderWindow* renWin =
    vtkOpenXRRenderWindow::SafeDownCast(this->Interactor->GetRenderWindow());
  if (!renWin ||
    (controller != vtkEventDataDevice::LeftController &&
      controller != vtkEventDataDevice::RightController))
  {
    return;
  }

  const int trackedDeviceIndex = renWin->GetTrackedDeviceIndexForDevice(controller);
  vtkVRModel* model = renWin->GetTrackedDeviceModel(trackedDeviceIndex);

  if (model)
  {
    model->SetShowRay(false);
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::UpdateRay(vtkEventDataDevice controller)
{
  if (!this->Interactor)
  {
    return;
  }

  vtkRenderer* ren = this->CurrentRenderer;
  vtkOpenXRRenderWindow* renWin =
    vtkOpenXRRenderWindow::SafeDownCast(this->Interactor->GetRenderWindow());

  this->CurrentRenderer = static_cast<vtkRenderer*>(renWin->GetRenderers()->GetItemAsObject(0));

  vtkOpenXRRenderWindowInteractor* iren =
    static_cast<vtkOpenXRRenderWindowInteractor*>(this->Interactor);

  if (!ren || !renWin || !iren)
  {
    return;
  }

  // Get the index from the controller
  const int trackedDeviceIndex = renWin->GetTrackedDeviceIndexForDevice(controller);
  vtkVRModel* model = renWin->GetTrackedDeviceModel(trackedDeviceIndex);

  if (!model)
  {
    return;
  }

  int idev = static_cast<int>(controller);

  // Keep the same ray if a controller is interacting with a prop
  if (this->InteractionProps[idev] != nullptr)
  {
    return;
  }

  // Check if interacting with a widget
  vtkPropCollection* props = ren->GetViewProps();

  vtkIdType nbProps = props->GetNumberOfItems();
  for (vtkIdType i = 0; i < nbProps; i++)
  {
    vtkWidgetRepresentation* rep = vtkWidgetRepresentation::SafeDownCast(props->GetItemAsObject(i));

    if (rep && rep->IsA("vtkQWidgetRepresentation") && rep->GetInteractionState() != 0)
    {
      model->SetShowRay(true);
      model->SetRayLength(ren->GetActiveCamera()->GetClippingRange()[1]);
      model->SetRayColor(0.0, 0.0, 1.0);
      return;
    }
  }

  if (this->GetGrabWithRay() || this->InteractionState[idev] == VTKIS_PICK)
  {
    model->SetShowRay(true);
  }
  else
  {
    model->SetShowRay(false);
    return;
  }

  // Set length to its max if interactive picking is off
  if (!this->HoverPick)
  {
    model->SetRayColor(1.0, 0.0, 0.0);
    model->SetRayLength(ren->GetActiveCamera()->GetClippingRange()[1]);
    return;
  }

  // Compute controller position and world orientation
  double p0[3];   // Ray start point
  double wxyz[4]; // Controller orientation
  double dummy_ppos[3];
  double wdir[3];
  XrPosef tdPose = iren->GetHandPose(trackedDeviceIndex);
  iren->ConvertOpenXRPoseToWorldCoordinates(tdPose, p0, wxyz, dummy_ppos, wdir);

  // Compute ray length.
  this->InteractionPicker->Pick3DRay(p0, wxyz, ren);

  // If something is picked, set the length accordingly
  vtkProp3D* prop = this->InteractionPicker->GetProp3D();
  if (prop)
  {
    double p1[3];
    this->InteractionPicker->GetPickPosition(p1);
    model->SetRayLength(sqrt(vtkMath::Distance2BetweenPoints(p0, p1)));
    model->SetRayColor(0.0, 1.0, 0.0);
  }
  // Otherwise set the length to its max
  else
  {
    model->SetRayLength(ren->GetActiveCamera()->GetClippingRange()[1]);
    model->SetRayColor(1.0, 0.0, 0.0);
  }

  return;
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::ShowBillboard(const std::string& vtkNotUsed(text))
{
  vtkOpenXRRenderWindow* renWin =
    vtkOpenXRRenderWindow::SafeDownCast(this->Interactor->GetRenderWindow());
  vtkRenderer* ren = this->CurrentRenderer;
  if (!renWin || !ren)
  {
    return;
  }

  vtkWarningMacro(<< "ShowBillboard");
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::HideBillboard()
{
  this->CurrentRenderer->RemoveActor(this->TextActor3D);
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::ShowPickSphere(double* pos, double radius, vtkProp3D* prop)
{
  this->PickActor->GetProperty()->SetColor(this->PickColor);

  this->Sphere->SetCenter(pos);
  this->Sphere->SetRadius(radius);
  this->PickActor->GetMapper()->SetInputConnection(this->Sphere->GetOutputPort());
  if (prop)
  {
    this->PickActor->SetPosition(prop->GetPosition());
    this->PickActor->SetScale(prop->GetScale());
  }
  else
  {
    this->PickActor->SetPosition(0.0, 0.0, 0.0);
    this->PickActor->SetScale(1.0, 1.0, 1.0);
  }
  this->CurrentRenderer->AddActor(this->PickActor);
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::ShowPickCell(vtkCell* cell, vtkProp3D* prop)
{
  vtkNew<vtkPolyData> pd;
  vtkNew<vtkPoints> pdpts;
  pdpts->SetDataTypeToDouble();
  vtkNew<vtkCellArray> lines;

  this->PickActor->GetProperty()->SetColor(this->PickColor);

  int nedges = cell->GetNumberOfEdges();

  if (nedges)
  {
    for (int edgenum = 0; edgenum < nedges; ++edgenum)
    {
      vtkCell* edge = cell->GetEdge(edgenum);
      vtkPoints* pts = edge->GetPoints();
      int npts = edge->GetNumberOfPoints();
      lines->InsertNextCell(npts);
      for (int ep = 0; ep < npts; ++ep)
      {
        vtkIdType newpt = pdpts->InsertNextPoint(pts->GetPoint(ep));
        lines->InsertCellPoint(newpt);
      }
    }
  }
  else if (cell->GetCellType() == VTK_LINE || cell->GetCellType() == VTK_POLY_LINE)
  {
    vtkPoints* pts = cell->GetPoints();
    int npts = cell->GetNumberOfPoints();
    lines->InsertNextCell(npts);
    for (int ep = 0; ep < npts; ++ep)
    {
      vtkIdType newpt = pdpts->InsertNextPoint(pts->GetPoint(ep));
      lines->InsertCellPoint(newpt);
    }
  }
  else
  {
    return;
  }

  pd->SetPoints(pdpts.Get());
  pd->SetLines(lines.Get());

  if (prop)
  {
    this->PickActor->SetPosition(prop->GetPosition());
    this->PickActor->SetScale(prop->GetScale());
    this->PickActor->SetUserMatrix(prop->GetUserMatrix());
  }
  else
  {
    this->PickActor->SetPosition(0.0, 0.0, 0.0);
    this->PickActor->SetScale(1.0, 1.0, 1.0);
  }
  this->PickActor->SetOrientation(prop->GetOrientation());
  static_cast<vtkPolyDataMapper*>(this->PickActor->GetMapper())->SetInputData(pd);
  this->CurrentRenderer->AddActor(this->PickActor);
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::HidePickActor()
{
  if (this->CurrentRenderer)
  {
    this->CurrentRenderer->RemoveActor(this->PickActor);
  }
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::AddTooltipForInput(
  vtkEventDataDevice device, vtkEventDataDeviceInput input)
{
  this->AddTooltipForInput(device, input, "");
}

//------------------------------------------------------------------------------
void vtkOpenXRInteractorStyle::AddTooltipForInput(vtkEventDataDevice vtkNotUsed(device),
  vtkEventDataDeviceInput vtkNotUsed(input), const std::string& vtkNotUsed(text))
{
  return;
}
