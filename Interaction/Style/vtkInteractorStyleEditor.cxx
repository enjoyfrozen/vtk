/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleEditor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.


  vtkInteractorStyleEditor is an interactor style resembling the behaviour of
  many 3d editors. For example Blender.

  The three mouse buttons are used as follows:

  Left :   selecting objects
  Middle : navigation
  Right :  reserved for context menu

  although the user can override this if required.

  The navigation assumes a clear vertical axis (0,0,1) that remains vertical. For
  many users this feels natural.

  This style borrows/collects many elements from exising functionality:
  - box select and area picker
  - pan
  - ....

  The only truely new routine is the camera rotation with is implemented in Rotate and
  is invoked via VTKIS_ROTATE

=========================================================================*/
#include "vtkInteractorStyleEditor.h"

#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkProp3D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkInteractorStyleEditor);

//------------------------------------------------------------------------------
vtkInteractorStyleEditor::vtkInteractorStyleEditor()
{
  this->MotionFactor = 10.0;
  this->InteractionProp = nullptr;
  this->InteractionPicker = vtkCellPicker::New();
  this->InteractionPicker->SetTolerance(0.001);
}

//------------------------------------------------------------------------------
vtkInteractorStyleEditor::~vtkInteractorStyleEditor()
{
  this->InteractionPicker->Delete();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::OnMouseMove()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  switch (this->State)
  {
    case VTKIS_ROTATE:
      this->FindPokedRenderer(x, y);
      this->Rotate();
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;

    case VTKIS_PAN:
      this->FindPokedRenderer(x, y);
      this->Pan();
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;

    case VTKIS_DOLLY:
      this->FindPokedRenderer(x, y);
      this->Dolly();
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;

    //case VTKIS_SPIN:
    //  this->FindPokedRenderer(x, y);
    //  this->Spin();
    //  this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
    //  break;

    case VTKIS_USCALE:
      this->FindPokedRenderer(x, y);
      this->UniformScale();
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::OnLeftButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  this->FindPickedActor(x, y);
  if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr)
  {
    return;
  }

  this->GrabFocus(this->EventCallbackCommand);
  if (this->Interactor->GetShiftKey())
  {
    this->StartPan();
  }
  else if (this->Interactor->GetControlKey())
  {
    this->StartSpin();
  }
  else
  {
    this->StartRotate();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::OnLeftButtonUp()
{
  switch (this->State)
  {
    case VTKIS_PAN:
      this->EndPan();
      break;

    case VTKIS_SPIN:
      this->EndSpin();
      break;

    case VTKIS_ROTATE:
      this->EndRotate();
      break;
  }

  if (this->Interactor)
  {
    this->ReleaseFocus();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::OnMiddleButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  this->FindPickedActor(x, y);
  if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr)
  {
    return;
  }

  this->GrabFocus(this->EventCallbackCommand);
  if (this->Interactor->GetControlKey())
  {
    this->StartDolly();
  }
  else
  {
    this->StartPan();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::OnMiddleButtonUp()
{
  switch (this->State)
  {
    case VTKIS_DOLLY:
      this->EndDolly();
      break;

    case VTKIS_PAN:
      this->EndPan();
      break;
  }

  if (this->Interactor)
  {
    this->ReleaseFocus();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::OnRightButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  this->FindPickedActor(x, y);
  if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr)
  {
    return;
  }

  this->GrabFocus(this->EventCallbackCommand);
this->StartUniformScale();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::OnRightButtonUp()
{
    switch (this->State)
    {
    case VTKIS_USCALE:
        this->EndUniformScale();
        break;
    }

    if (this->Interactor)
    {
        this->ReleaseFocus();
    }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::Rotate()
{
    if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr)
    {
        return;
    }

    /*
    CurrentRenderer = self.GetCurrentRenderer()

          if CurrentRenderer:

              rwi = self.GetInteractor()
              dx = rwi.GetEventPosition()[0] - rwi.GetLastEventPosition()[0]
              dy = rwi.GetEventPosition()[1] - rwi.GetLastEventPosition()[1]

              size = CurrentRenderer.GetRenderWindow().GetSize()
              delta_elevation = -20.0 / size[1]
              delta_azimuth = -20.0 / size[0]

              rxf = dx * delta_azimuth * self.MOUSE_MOTION_FACTOR
              ryf = dy * delta_elevation * self.MOUSE_MOTION_FACTOR

              self.RotateTurtableBy(rxf, ryf)*/

    vtkRenderWindowInteractor* rwi = this->Interactor;

    const int dx{ rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0] };
    const int dy{ rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1] };

    const int width{ this->CurrentRenderer->GetSize()[0] };
    const int height{ this->CurrentRenderer->GetSize()[1] };

    const double delta_elevation{ -20. / height };
    const double delta_azimuth{ -20. / width };

    const double MOUSE_MOTION_FACTOR{ 20 };

    RotateTurntableBy(
        dx * delta_azimuth * MOUSE_MOTION_FACTOR, dy * delta_elevation * MOUSE_MOTION_FACTOR);
}


void vtkInteractorStyleEditor::RotateTurntableBy(float rxf, float ryf)
{
  /*
  * rfx is rotation about the global Z vector (turn-table mode)
  * rfy is rotation about the side vector
  *
  * CurrentRenderer = self.GetCurrentRenderer()
        rwi = self.GetInteractor()
  */
 
  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();

  double* campos{ camera->GetPosition() };
  double* focal{ camera->GetFocalPoint() };

  double* up{ camera->GetViewUp() };
  double upside_down_factor{ up[2] < 0 ? -1. : 1. };

  // P = campos - focal  # camera position

  const double Px{ campos[0] - focal[0] };
  const double Py{ campos[1] - focal[1] };
  const double Pz{ campos[2] - focal[2] };

  const double H{ sqrt(pow(Px, 2) + pow(Py, 2)) };
  const double elev{ atan2(Pz, H) };

  //      # Rotate left/right about the global Z axis
  //      H = np.linalg.norm(P[:2])     # horizontal distance of camera to focal point
  //      elev = np.arctan2(P[2], H)    # elevation

  //      # if the camera is near the poles, then derive the azimuth from the up-vector

  const double sin_elev{ sin(elev) };
  double azi{ 0 };

  if (abs(sin_elev) < 0.8)
  {
    azi = atan2(Py, Px);
  }
  else
  {
    if (sin_elev < -0.8)
    {
      azi = atan2(upside_down_factor * up[1], upside_down_factor * up[0]);
    }
    else
    {
      azi = atan2(-upside_down_factor * up[1], -upside_down_factor * up[0]);
    }
  }

  // distance from focal point to camera

  const double D{ sqrt(pow(Px, 2) + pow(Py, 2) + pow(Pz, 2)) };

  //    # apply the change in azimuth and elevation
  const double azi_new {  azi + rxf / 60.};

  const double elev_new{ elev + upside_down_factor * ryf / 60. };


        //# the changed elevation changes H (D stays the same)
  const double H_new{ D * cos(elev_new) };


        //# calculate new camera position relative to focal point
    //Pnew = np.array((Hnew * np.cos(azi_new), Hnew * np.sin(azi_new), D * np.sin(elev_new)))

    const double Px_new{ H_new * cos(azi_new) };
    const double Py_new{ H_new * sin(azi_new) };
    const double Pz_new{ D * sin(elev_new) };


    // # calculate the up-direction of the camera
    const double up_z{ upside_down_factor * cos(elev_new) };  // z follows directly from elevation
    const double up_h{ upside_down_factor * sin(elev_new) };  // horizontal component
        

//        up = (-up_h * np.cos(azi_new),
//              -up_h * np.sin(azi_new),
//              up_z)

//        new_pos = focal + Pnew

    camera->SetViewUp(-up_h * cos(azi_new), -up_h * sin(azi_new), up_z);
    camera->SetPosition(focal[0] + Px_new, 
        focal[1] + Py_new, 
        focal[2] + Pz_new);
    


        //camera.SetViewUp(up)
        //camera.SetPosition(new_pos)

    camera->OrthogonalizeViewUp();


  vtkRenderWindowInteractor* rwi = this->Interactor;


    if (this->AutoAdjustCameraClippingRange)
    {
      this->CurrentRenderer->ResetCameraClippingRange();
    }

    rwi->Render();
  }


//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::Spin()
{
 // No spin
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::Pan()
{
  if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;

  // Use initial center as the origin from which to pan

  double* obj_center = this->InteractionProp->GetCenter();

  double disp_obj_center[3], new_pick_point[4];
  double old_pick_point[4], motion_vector[3];

  this->ComputeWorldToDisplay(obj_center[0], obj_center[1], obj_center[2], disp_obj_center);

  this->ComputeDisplayToWorld(
    rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], disp_obj_center[2], new_pick_point);

  this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0], rwi->GetLastEventPosition()[1],
    disp_obj_center[2], old_pick_point);

  motion_vector[0] = new_pick_point[0] - old_pick_point[0];
  motion_vector[1] = new_pick_point[1] - old_pick_point[1];
  motion_vector[2] = new_pick_point[2] - old_pick_point[2];

  if (this->InteractionProp->GetUserMatrix() != nullptr)
  {
    vtkTransform* t = vtkTransform::New();
    t->PostMultiply();
    t->SetMatrix(this->InteractionProp->GetUserMatrix());
    t->Translate(motion_vector[0], motion_vector[1], motion_vector[2]);
    this->InteractionProp->GetUserMatrix()->DeepCopy(t->GetMatrix());
    t->Delete();
  }
  else
  {
    this->InteractionProp->AddPosition(motion_vector[0], motion_vector[1], motion_vector[2]);
  }

  if (this->AutoAdjustCameraClippingRange)
  {
    this->CurrentRenderer->ResetCameraClippingRange();
  }

  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::Dolly()
{
  if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;
  vtkCamera* cam = this->CurrentRenderer->GetActiveCamera();

  double view_point[3], view_focus[3];
  double motion_vector[3];

  cam->GetPosition(view_point);
  cam->GetFocalPoint(view_focus);

  double* center = this->CurrentRenderer->GetCenter();

  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  double yf = dy / center[1] * this->MotionFactor;
  double dollyFactor = pow(1.1, yf);

  dollyFactor -= 1.0;
  motion_vector[0] = (view_point[0] - view_focus[0]) * dollyFactor;
  motion_vector[1] = (view_point[1] - view_focus[1]) * dollyFactor;
  motion_vector[2] = (view_point[2] - view_focus[2]) * dollyFactor;

  if (this->InteractionProp->GetUserMatrix() != nullptr)
  {
    vtkTransform* t = vtkTransform::New();
    t->PostMultiply();
    t->SetMatrix(this->InteractionProp->GetUserMatrix());
    t->Translate(motion_vector[0], motion_vector[1], motion_vector[2]);
    this->InteractionProp->GetUserMatrix()->DeepCopy(t->GetMatrix());
    t->Delete();
  }
  else
  {
    this->InteractionProp->AddPosition(motion_vector);
  }

  if (this->AutoAdjustCameraClippingRange)
  {
    this->CurrentRenderer->ResetCameraClippingRange();
  }

  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::UniformScale()
{
  if (this->CurrentRenderer == nullptr || this->InteractionProp == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;

  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

  double* obj_center = this->InteractionProp->GetCenter();
  double* center = this->CurrentRenderer->GetCenter();

  double yf = dy / center[1] * this->MotionFactor;
  double scaleFactor = pow(1.1, yf);

  double** rotate = nullptr;

  double scale[3];
  scale[0] = scale[1] = scale[2] = scaleFactor;

  this->Prop3DTransform(this->InteractionProp, obj_center, 0, rotate, scale);

  if (this->AutoAdjustCameraClippingRange)
  {
    this->CurrentRenderer->ResetCameraClippingRange();
  }

  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::FindPickedActor(int x, int y)
{
  this->InteractionPicker->Pick(x, y, 0.0, this->CurrentRenderer);
  vtkProp* prop = this->InteractionPicker->GetViewProp();
  if (prop != nullptr)
  {
    this->InteractionProp = vtkProp3D::SafeDownCast(prop);
  }
  else
  {
    this->InteractionProp = nullptr;
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::Prop3DTransform(
  vtkProp3D* prop3D, double* boxCenter, int numRotation, double** rotate, double* scale)
{
  vtkMatrix4x4* oldMatrix = vtkMatrix4x4::New();
  prop3D->GetMatrix(oldMatrix);

  double orig[3];
  prop3D->GetOrigin(orig);

  vtkTransform* newTransform = vtkTransform::New();
  newTransform->PostMultiply();
  if (prop3D->GetUserMatrix() != nullptr)
  {
    newTransform->SetMatrix(prop3D->GetUserMatrix());
  }
  else
  {
    newTransform->SetMatrix(oldMatrix);
  }

  newTransform->Translate(-(boxCenter[0]), -(boxCenter[1]), -(boxCenter[2]));

  for (int i = 0; i < numRotation; i++)
  {
    newTransform->RotateWXYZ(rotate[i][0], rotate[i][1], rotate[i][2], rotate[i][3]);
  }

  if ((scale[0] * scale[1] * scale[2]) != 0.0)
  {
    newTransform->Scale(scale[0], scale[1], scale[2]);
  }

  newTransform->Translate(boxCenter[0], boxCenter[1], boxCenter[2]);

  // now try to get the composite of translate, rotate, and scale
  newTransform->Translate(-(orig[0]), -(orig[1]), -(orig[2]));
  newTransform->PreMultiply();
  newTransform->Translate(orig[0], orig[1], orig[2]);

  if (prop3D->GetUserMatrix() != nullptr)
  {
    newTransform->GetMatrix(prop3D->GetUserMatrix());
  }
  else
  {
    prop3D->SetPosition(newTransform->GetPosition());
    prop3D->SetScale(newTransform->GetScale());
    prop3D->SetOrientation(newTransform->GetOrientation());
  }
  oldMatrix->Delete();
  newTransform->Delete();
}
VTK_ABI_NAMESPACE_END
