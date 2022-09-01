/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleTrackballCamera.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkMatrix3x3.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkInteractorStyleTrackballCamera);

//------------------------------------------------------------------------------
vtkInteractorStyleTrackballCamera::vtkInteractorStyleTrackballCamera() {}

//------------------------------------------------------------------------------
vtkInteractorStyleTrackballCamera::~vtkInteractorStyleTrackballCamera() = default;

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnMouseMove()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  switch (this->State)
  {
    case VTKIS_ENV_ROTATE:
      this->FindPokedRenderer(x, y);
      this->EnvironmentRotate();
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;

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

    case VTKIS_SPIN:
      this->FindPokedRenderer(x, y);
      this->Spin();
      this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
      break;
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnLeftButtonDown()
{
  this->FindPokedRenderer(
    this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  this->SetLeftButtonDownPosition(this->Interactor->GetEventPosition());
  this->GrabFocus(this->EventCallbackCommand);

  if (this->Interactor->GetShiftKey() || !this->RotationEnabled)
  {
    if (this->Interactor->GetControlKey())
    {
      this->StartDolly();
    }
    else
    {
      this->StartPan();
    }
  }
  else
  {
    if (this->Interactor->GetControlKey())
    {
      this->StartSpin();
    }
    else
    {
      this->StartRotate();
    }
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnLeftButtonUp()
{
  switch (this->State)
  {
    case VTKIS_DOLLY:
      this->EndDolly();
      break;

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

  double startPoint[3] = { static_cast<double>(this->LeftButtonDownPosition[0]),
    static_cast<double>(this->LeftButtonDownPosition[1]), 0.0 };
  double endPoint[3] = { static_cast<double>(this->Interactor->GetEventPosition()[0]),
    static_cast<double>(this->Interactor->GetEventPosition()[1]), 0.0 };
  double distanceSquared = vtkMath::Distance2BetweenPoints(startPoint, endPoint);
  if (distanceSquared <= this->ClickTolerance * this->ClickTolerance)
  {
    this->OnLeftButtonSingleClick();
  }

  if (this->Interactor)
  {
    this->ReleaseFocus();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnLeftButtonSingleClick() {}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnMiddleButtonDown()
{
  this->FindPokedRenderer(
    this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  this->SetMiddleButtonDownPosition(this->Interactor->GetEventPosition());
  this->GrabFocus(this->EventCallbackCommand);
  this->StartPan();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnMiddleButtonUp()
{
  switch (this->State)
  {
    case VTKIS_PAN:
      this->EndPan();
      if (this->Interactor)
      {
        this->ReleaseFocus();
      }
      break;
  }

  double startPoint[3] = { static_cast<double>(this->MiddleButtonDownPosition[0]),
    static_cast<double>(this->MiddleButtonDownPosition[1]), 0.0 };
  double endPoint[3] = { static_cast<double>(this->Interactor->GetEventPosition()[0]),
    static_cast<double>(this->Interactor->GetEventPosition()[1]), 0.0 };
  double distanceSquared = vtkMath::Distance2BetweenPoints(startPoint, endPoint);
  if (distanceSquared <= this->ClickTolerance * this->ClickTolerance)
  {
    this->OnMiddleButtonSingleClick();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnMiddleButtonSingleClick() {}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnRightButtonDown()
{
  this->FindPokedRenderer(
    this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  this->SetRightButtonDownPosition(this->Interactor->GetEventPosition());
  this->GrabFocus(this->EventCallbackCommand);

  if (this->Interactor->GetShiftKey())
  {
    this->StartEnvRotate();
  }
  else
  {
    this->StartDolly();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnRightButtonUp()
{
  switch (this->State)
  {
    case VTKIS_ENV_ROTATE:
      this->EndEnvRotate();
      break;

    case VTKIS_DOLLY:
      this->EndDolly();
      break;
  }

  double startPoint[3] = { static_cast<double>(this->RightButtonDownPosition[0]),
    static_cast<double>(this->RightButtonDownPosition[1]), 0.0 };
  double endPoint[3] = { static_cast<double>(this->Interactor->GetEventPosition()[0]),
    static_cast<double>(this->Interactor->GetEventPosition()[1]), 0.0 };
  double distanceSquared = vtkMath::Distance2BetweenPoints(startPoint, endPoint);
  if (distanceSquared <= this->ClickTolerance * this->ClickTolerance)
  {
    this->OnRightButtonSingleClick();
  }

  if (this->Interactor)
  {
    this->ReleaseFocus();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnRightButtonSingleClick() {}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnMouseWheelForward()
{
  this->OnMouseWheelAction(1.0);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnMouseWheelBackward()
{
  this->OnMouseWheelAction(-1.0);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::OnMouseWheelAction(double direction)
{
  this->FindPokedRenderer(
    this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  this->GrabFocus(this->EventCallbackCommand);
  this->StartDolly();
  double factor = this->MotionFactor * direction * 0.2 * this->MouseWheelMotionFactor;
  factor = pow(1.1, factor);
  if (this->MouseWheelInvertDirection && factor != 0.0)
  {
    factor = 1.0f / factor;
  }
  if (vtkInteractorStyleCameraUtils::IsParallelProjectionZoomingValid(
        this->CurrentRenderer, (direction > 0.0)))
  {
    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    if (camera != nullptr && camera->GetParallelProjection() &&
      this->DollyModel == VTK_DOLLY_MODEL_TARGETTED)
    {
      vtkInteractorStyleCameraUtils::DollyTargetted(
        this->Interactor, this->CurrentRenderer, factor);
    }
    else
    {
      this->Dolly(factor);
    }
  }
  this->EndDolly();
  this->ReleaseFocus();

  vtkRenderWindowInteractor* rwi = this->Interactor;
  if (this->AutoAdjustCameraClippingRange)
  {
    this->CurrentRenderer->ResetCameraClippingRange();
  }
  if (rwi->GetLightFollowCamera())
  {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
  }
  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::Rotate()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  switch (this->RotationModel)
  {
    case VTK_TRACKBALL_ROTATION_SINGULARITY:
      this->RotateSingularityCalculateAxisAndAngle();
      this->RotateSingularity();
      break;
    case VTK_TRACKBALL_ROTATION_WORLDZ_SCREENX:
      this->RotateWorldZScreenX();
      break;
    case VTK_TRACKBALL_ROTATION_DEFAULT:
    default:
      this->RotateDefault();
      break;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;
  if (this->AutoAdjustCameraClippingRange)
  {
    this->CurrentRenderer->ResetCameraClippingRange();
  }
  if (rwi->GetLightFollowCamera())
  {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
  }
  rwi->Render();
}

//------------------------------------------------------------------------------
bool vtkInteractorStyleTrackballCamera::CanRepeatRotation()
{
  if (this->CurrentRenderer == nullptr)
  {
    return false;
  }

  switch (this->RotationModel)
  {
    case VTK_TRACKBALL_ROTATION_SINGULARITY:
      return (std::abs(this->SingularityRotationAngle) >= (std::numeric_limits<float>::min)() &&
        std::isnormal(this->SingularityRotationAngle));
    case VTK_TRACKBALL_ROTATION_WORLDZ_SCREENX:
      return (std::abs(this->ConstrainedRotationPhi) >= std::numeric_limits<float>::min()) ||
        (std::abs(this->ConstrainedRotationTheta) >= std::numeric_limits<float>::min());
    case VTK_TRACKBALL_ROTATION_DEFAULT:
    default:
      return false;
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::RepeatRotation()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  switch (this->RotationModel)
  {
    case VTK_TRACKBALL_ROTATION_SINGULARITY:
      this->RotateSingularity();
      break;
    case VTK_TRACKBALL_ROTATION_WORLDZ_SCREENX:
      vtkInteractorStyleCameraUtils::RotateCameraAroundWorldZScreenX(this->CurrentRenderer,
        this->ConstrainedRotationPhi, this->ConstrainedRotationTheta, false, false);
      break;
    case VTK_TRACKBALL_ROTATION_DEFAULT:
    default:
      vtkErrorMacro(
        "RotateAgain not implemented for this RotationModel (VTK_TRACKBALL_ROTATION_DEFAULT)");
      break;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;
  if (this->AutoAdjustCameraClippingRange)
  {
    this->CurrentRenderer->ResetCameraClippingRange();
  }
  if (rwi->GetLightFollowCamera())
  {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
  }
  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::RotateDefault()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;

  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

  const int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  double delta_elevation = -20.0 / size[1];
  double delta_azimuth = -20.0 / size[0];

  double rxf = dx * delta_azimuth * this->MotionFactor;
  double ryf = dy * delta_elevation * this->MotionFactor;

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
  camera->Azimuth(rxf);
  camera->Elevation(ryf);
  camera->OrthogonalizeViewUp();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::RotateSingularity()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  // Ignoring rotation angles that are too small or invalid
  if (CanRepeatRotation() == false)
  {
    return;
  }

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();

  // NOTE: Retrieving the camera light transform matrix in order to transform the rotation axis to
  //       the camera coordinate frame instead of the model view matrix. This makes the rotation
  //       code simpler and more reliable, as there were issues when the VTK model view matrix was
  //       used as below.
  // NOTE: The model view matrix needs to be inverted in order to obtain the camera transformation
  //       (see https://www.3dgep.com/understanding-the-view-matrix/). This is needed because VTK
  //       calculates the model view matrix from the camera settings (position, focal point, and up
  //       vector) and overwrites the matrix when these change. So the camera settings have to
  //       updated instead of directly changing the model view matrix. In order to do this, the
  //       previously calculated axis of rotation has to be transformed from the world coordinate
  //       frame to the camera's, given by the camera transformation.
  vtkNew<vtkMatrix4x4> modelViewMatrix;
  modelViewMatrix->DeepCopy(camera->GetCameraLightTransformMatrix());

  // Creating the rotation transform to be applied to the camera parameters by applying the rotation
  // about the trackball rotation axis to the camera frame matrix
  vtkNew<vtkTransform> rotationTransform;
  rotationTransform->SetMatrix(modelViewMatrix);
  rotationTransform->RotateWXYZ(this->SingularityRotationAngle, this->SingularityRotationAxis[0],
    this->SingularityRotationAxis[1], this->SingularityRotationAxis[2]);

  // Calculating the new camera position
  double canonicalCameraPosition[3] = { 0.0, 0.0, 1.0 };
  double newCameraPosition[3] = { 0.0, 0.0, 0.0 };
  rotationTransform->TransformPoint(canonicalCameraPosition, newCameraPosition);

  // Calculating the new view up vector
  double canonicalViewUp[3] = { 0.0, 1.0, 0.0 };
  double newViewUp[3] = { 0.0, 0.0, 0.0 };
  rotationTransform->TransformVector(canonicalViewUp, newViewUp);
  vtkMath::Normalize(newViewUp);

  // Making sure the new camera position is valid before assigning it to the camera
  if (std::isnan(newCameraPosition[0]) || std::isnan(newCameraPosition[1]) ||
    std::isnan(newCameraPosition[2]))
  {
    this->SingularityRotationAngle = 0.0f;
    return;
  }

  // Updating the camera with the new position and view up vector
  camera->SetPosition(newCameraPosition);
  camera->SetViewUp(newViewUp);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::RotateSingularityCalculateAxisAndAngle()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  // Getting the viewport dimensions
  int* viewportSize = this->CurrentRenderer->GetRenderWindow()->GetSize();
  const int viewportWidth = viewportSize[0];
  const int viewportHeight = viewportSize[1];

  // Obtaining the center of the viewport as the origin
  const double halfWidth = viewportWidth * 0.5;
  const double halfHeight = viewportHeight * 0.5;

  // Getting the adjusted position of the previous and current mouse events
  const int previousX = this->Interactor->GetLastEventPosition()[0];
  const int previousY = this->Interactor->GetLastEventPosition()[1];
  const int currentX = this->Interactor->GetEventPosition()[0];
  const int currentY = this->Interactor->GetEventPosition()[1];

  // If the mouse cursor has not really moved, do nothing
  if ((currentX - previousX == 0) && (currentY - previousY == 0))
  {
    return;
  }

  // Using the diagonal of the viewport as the radius of the trackball hemisphere (to avoid dead
  // zones at the corners, resulting from the hemiellipsoid given by distinct width and height
  // values)
  const double halfDiagonalLength = std::sqrt(halfWidth * halfWidth + halfHeight * halfHeight);

  // Projecting previous coordinates on a hemisphere with unit radius
  const double x1 = (static_cast<double>(previousX) - halfWidth) / halfDiagonalLength;
  const double y1 = (static_cast<double>(previousY) - halfHeight) / halfDiagonalLength;
  double z1 = 1.0 - x1 * x1 - y1 * y1;
  // Checking for a negative number in the square root
  z1 = (z1 <= 0.0 ? 0.0 : std::sqrt(z1));

  // Projecting current coordinates on a hemisphere with unit radius
  const double x2 = (static_cast<double>(currentX) - halfWidth) / halfDiagonalLength;
  const double y2 = (static_cast<double>(currentY) - halfHeight) / halfDiagonalLength;
  double z2 = 1.0 - x2 * x2 - y2 * y2;
  // Checking for a negative number in the square root
  z2 = (z2 <= 0.0 ? 0.0 : std::sqrt(z2));

  // Taking the cross-product between the vectors defined by the two
  // previous projections to find the axis of rotation (it is normal to
  // the plane defined by the vectors between the center of the
  // hemisphere and the projected points).
  this->SingularityRotationAxis[0] = y1 * z2 - y2 * z1;
  this->SingularityRotationAxis[1] = z1 * x2 - z2 * x1;
  this->SingularityRotationAxis[2] = x1 * y2 - x2 * y1;

  // Calculating the square of the rotation axis length
  double rotationAxisSquaredLength = vtkMath::SquaredNorm(this->SingularityRotationAxis);
  if (rotationAxisSquaredLength < 0.0)
  {
    rotationAxisSquaredLength = 0.0;
  }
  // Calculating the rotation angle in degrees from the magnitude of the
  // (pseudo) vector resulting from the cross-product (since both
  // vectors have unit length, i.e., are already normalized).
  const double norm = std::sqrt(rotationAxisSquaredLength);
  this->SingularityRotationAngle = static_cast<double>(std::asin(norm) * 180.0 / vtkMath::Pi());

  // Normalizing the rotation axis
  if (norm != 0.0)
  {
    vtkMath::MultiplyScalar(this->SingularityRotationAxis, 1.0 / norm);
  }

  // The rotation angle needs to be inverted because it is the camera position that is being
  // rotated, not the viewed object.
  this->SingularityRotationAngle *= -1.0;

  // Adjusting the rotation sensitivity by applying the motion factor to the rotation angle
  this->SingularityRotationAngle *= this->MotionFactorSingularityRotation;
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::RotateWorldZScreenX()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;

  int* size = this->CurrentRenderer->GetRenderWindow()->GetSize();
  double vpWidth = size[0];
  double vpHeight = size[1];
  if (vpWidth == 0.0 || vpHeight == 0.0)
  {
    this->ConstrainedRotationTheta = 0.0;
    this->ConstrainedRotationPhi = 0.0;
    return;
  }

  int plx = rwi->GetLastEventPosition()[0];
  int ply = static_cast<int>(vpHeight) - rwi->GetLastEventPosition()[1];
  int pcx = rwi->GetEventPosition()[0];
  int pcy = static_cast<int>(vpHeight) - rwi->GetEventPosition()[1];

  this->ConstrainedRotationTheta = -2.0 * static_cast<double>(plx - pcx) / vpWidth * 180.0;
  this->ConstrainedRotationPhi = static_cast<double>(pcy - ply) / vpHeight * 180.0;

  vtkInteractorStyleCameraUtils::RotateCameraAroundWorldZScreenX(this->CurrentRenderer,
    this->ConstrainedRotationPhi, this->ConstrainedRotationTheta, false, false);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::Spin()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;

  double* center = this->CurrentRenderer->GetCenter();

  double newAngle = vtkMath::DegreesFromRadians(
    atan2(rwi->GetEventPosition()[1] - center[1], rwi->GetEventPosition()[0] - center[0]));

  double oldAngle = vtkMath::DegreesFromRadians(
    atan2(rwi->GetLastEventPosition()[1] - center[1], rwi->GetLastEventPosition()[0] - center[0]));

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
  camera->Roll(newAngle - oldAngle);
  camera->OrthogonalizeViewUp();

  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::Pan()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;

  double viewFocus[4], focalDepth, viewPoint[3];
  double newPickPoint[4], oldPickPoint[4], motionVector[3];

  // Calculate the focal depth since we'll be using it a lot

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
  camera->GetFocalPoint(viewFocus);
  this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
  focalDepth = viewFocus[2];

  this->ComputeDisplayToWorld(
    rwi->GetEventPosition()[0], rwi->GetEventPosition()[1], focalDepth, newPickPoint);

  // Has to recalc old mouse point since the viewport has moved,
  // so can't move it outside the loop

  this->ComputeDisplayToWorld(
    rwi->GetLastEventPosition()[0], rwi->GetLastEventPosition()[1], focalDepth, oldPickPoint);

  // Camera motion is reversed

  motionVector[0] = oldPickPoint[0] - newPickPoint[0];
  motionVector[1] = oldPickPoint[1] - newPickPoint[1];
  motionVector[2] = oldPickPoint[2] - newPickPoint[2];

  camera->GetFocalPoint(viewFocus);
  camera->GetPosition(viewPoint);
  camera->SetFocalPoint(
    motionVector[0] + viewFocus[0], motionVector[1] + viewFocus[1], motionVector[2] + viewFocus[2]);

  camera->SetPosition(
    motionVector[0] + viewPoint[0], motionVector[1] + viewPoint[1], motionVector[2] + viewPoint[2]);

  if (rwi->GetLightFollowCamera())
  {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
  }

  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::Dolly()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;
  double* center = this->CurrentRenderer->GetCenter();
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  double dyf = this->MotionFactor * dy / center[1];
  double factor = pow(1.1, dyf);
  if (this->MouseWheelInvertDirection && factor != 0.0)
  {
    factor = 1.0f / factor;
  }
  this->Dolly(factor);
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::Dolly(double factor)
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
  if (camera->GetParallelProjection())
  {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
  }
  else
  {
    camera->Dolly(factor);
    if (this->AutoAdjustCameraClippingRange)
    {
      this->CurrentRenderer->ResetCameraClippingRange();
    }
  }

  if (this->Interactor->GetLightFollowCamera())
  {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
  }

  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::EnvironmentRotate()
{
  if (this->CurrentRenderer == nullptr)
  {
    return;
  }

  vtkRenderWindowInteractor* rwi = this->Interactor;

  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int sizeX = this->CurrentRenderer->GetRenderWindow()->GetSize()[0];

  vtkNew<vtkMatrix3x3> mat;

  double* up = this->CurrentRenderer->GetEnvironmentUp();
  double* right = this->CurrentRenderer->GetEnvironmentRight();

  double front[3];
  vtkMath::Cross(right, up, front);
  for (int i = 0; i < 3; i++)
  {
    mat->SetElement(i, 0, right[i]);
    mat->SetElement(i, 1, up[i]);
    mat->SetElement(i, 2, front[i]);
  }

  double angle = (dx / static_cast<double>(sizeX)) * this->MotionFactor;

  double c = std::cos(angle);
  double s = std::sin(angle);
  double t = 1.0 - c;

  vtkNew<vtkMatrix3x3> rot;

  rot->SetElement(0, 0, t * up[0] * up[0] + c);
  rot->SetElement(0, 1, t * up[0] * up[1] - up[2] * s);
  rot->SetElement(0, 2, t * up[0] * up[2] + up[1] * s);

  rot->SetElement(1, 0, t * up[0] * up[1] + up[2] * s);
  rot->SetElement(1, 1, t * up[1] * up[1] + c);
  rot->SetElement(1, 2, t * up[1] * up[2] - up[0] * s);

  rot->SetElement(2, 0, t * up[0] * up[2] - up[1] * s);
  rot->SetElement(2, 1, t * up[1] * up[2] + up[0] * s);
  rot->SetElement(2, 2, t * up[2] * up[2] + c);

  vtkMatrix3x3::Multiply3x3(rot, mat, mat);

  // update environment orientation
  this->CurrentRenderer->SetEnvironmentUp(
    mat->GetElement(0, 1), mat->GetElement(1, 1), mat->GetElement(2, 1));
  this->CurrentRenderer->SetEnvironmentRight(
    mat->GetElement(0, 0), mat->GetElement(1, 0), mat->GetElement(2, 0));

  rwi->Render();
}

//------------------------------------------------------------------------------
void vtkInteractorStyleTrackballCamera::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "MotionFactor: " << this->MotionFactor << "\n";
}
VTK_ABI_NAMESPACE_END
