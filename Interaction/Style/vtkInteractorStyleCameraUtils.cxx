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
#include "vtkInteractorStyleCameraUtils.h"

#include <vtkCamera.h>
#include <vtkNew.h>
#include <vtkTransform.h>

//------------------------------------------------------------------------------
constexpr double vtkInteractorStyleCameraUtils::MINIMUM_PARALLEL_SCALE_VALUE = 1.0e-16;
constexpr double vtkInteractorStyleCameraUtils::MAXIMUM_PARALLEL_SCALE_VALUE = 1.0e100;

//------------------------------------------------------------------------------
bool vtkInteractorStyleCameraUtils::IsParallelProjectionZoomingWithinBounds(
  vtkZoomDirection zoomDirection, double parallelScale)
{
  return ((zoomDirection == vtkZoomDirection::ZoomingIn &&
            parallelScale > MINIMUM_PARALLEL_SCALE_VALUE) ||
    (zoomDirection != vtkZoomDirection::ZoomingIn && parallelScale < MAXIMUM_PARALLEL_SCALE_VALUE));
}

//------------------------------------------------------------------------------
bool vtkInteractorStyleCameraUtils::IsParallelProjectionZoomingValid(
  vtkRenderer* renderer, vtkZoomDirection zoomDirection)
{
  if (renderer == nullptr)
  {
    return false;
  }

  vtkCamera* camera = renderer->GetActiveCamera();
  if (camera == nullptr)
  {
    return false;
  }

  if (camera->GetParallelProjection())
  {
    // If the parallel scale value is not within the allowed limits, stop zooming
    if (!vtkInteractorStyleCameraUtils::IsParallelProjectionZoomingWithinBounds(
          zoomDirection, camera->GetParallelScale()))
    {
      return false;
    }

    // Getting the world coordinates of the current renderer boundaries to make sure that further
    // zooming in doesn't go beyond numeric representation capacity
    double worldLeftBottom[3] = { 0.0, 0.0, 0.0 };
    renderer->NormalizedViewportToWorld(worldLeftBottom[0], worldLeftBottom[1], worldLeftBottom[2]);
    double worldRightTop[3] = { 1.0, 1.0, 1.0 };
    renderer->NormalizedViewportToWorld(worldRightTop[0], worldRightTop[1], worldRightTop[2]);
    std::array<int, 3> boundaryEqualityValues = {
      (fabs(worldLeftBottom[0] - worldRightTop[0]) <= 1.0E-12) ? 1 : 0,
      (fabs(worldLeftBottom[1] - worldRightTop[1]) <= 1.0E-12) ? 1 : 0,
      (fabs(worldLeftBottom[2] - worldRightTop[2]) <= 1.0E-12) ? 1 : 0,
    };
    const int numberOfEqualBoundaries =
      boundaryEqualityValues[0] + boundaryEqualityValues[1] + boundaryEqualityValues[2];
    // For cross-sectional views, it is expected that one of the coordinate axes will be collapsed
    // to a range of zero, so if one more axis has also a range of zero, then zooming should be
    // stopped
    if (zoomDirection == vtkZoomDirection::ZoomingIn && numberOfEqualBoundaries > 1)
    {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
void vtkInteractorStyleCameraUtils::RotateCameraAroundWorldZScreenX(vtkRenderer* renderer,
  double phi, double theta, bool autoAdjustCameraClippingRange, bool lightFollowCamera)
{
  if (renderer == nullptr)
  {
    return;
  }

  vtkCamera* camera = renderer->GetActiveCamera();
  double* fp = camera->GetFocalPoint();

  vtkNew<vtkTransform> transform;
  transform->Identity();
  transform->RotateZ(-theta);

  double* viewUp = camera->GetViewUp();
  double* projectionDir = camera->GetDirectionOfProjection();
  double axis[3];
  vtkMath::Cross(projectionDir, viewUp, axis);
  transform->RotateWXYZ(-phi, axis);

  // Transforms to rotate around the focal point instead of the origin. They must be separate
  // vtkTransforms from ones which do rotation.
  vtkNew<vtkTransform> translateFPNeg;
  translateFPNeg->Identity();
  translateFPNeg->Translate(-fp[0], -fp[1], -fp[2]);

  vtkNew<vtkTransform> translateFPPos;
  translateFPPos->Identity();
  translateFPPos->Translate(fp[0], fp[1], fp[2]);

  camera->ApplyTransform(translateFPNeg);
  camera->ApplyTransform(transform);
  camera->ApplyTransform(translateFPPos);

  if (autoAdjustCameraClippingRange)
  {
    renderer->ResetCameraClippingRange();
  }

  if (lightFollowCamera)
  {
    renderer->UpdateLightsGeometryToFollowCamera();
  }
}

//------------------------------------------------------------------------------
void vtkInteractorStyleCameraUtils::DollyTargeted(
  vtkRenderWindowInteractor* interactor, vtkRenderer* renderer, double factor)
{
  vtkCamera* camera = renderer->GetActiveCamera();
  if (camera == nullptr)
  {
    return;
  }

  const int* screenPosition = interactor->GetEventPosition();
  const int* screenSize = interactor->GetSize();
  const double* viewport = renderer->GetViewport();

  // Adjusting the camera parallel projection scaling (i.e., the height of the viewport in world
  // coordinates)
  double parallelScale = camera->GetParallelScale();
  double newParallelScale = camera->GetParallelScale() / factor;
  camera->SetParallelScale(newParallelScale);

  // If either the screen or the viewport have one of their dimensions as zero, nothing should be
  // done
  if (screenSize[0] == 0 || screenSize[1] == 0 || (viewport[2] - viewport[0]) == 0.0 ||
    (viewport[3] - viewport[1]) == 0.0)
  {
    return;
  }

  // Make sure the mouse is inside the viewport (Don't zoom on ruler bars)
  double normX = static_cast<double>(screenPosition[0]) / static_cast<double>(screenSize[0]);
  double normY = static_cast<double>(screenPosition[1]) / static_cast<double>(screenSize[1]);
  if (normX < viewport[0] || normX > viewport[2] || normY < viewport[1] || normY > viewport[3])
  {
    return; // Mouse outside viewport
  }

  // Determine the normalized points to center around
  // Converting the mouse position x-coordinate to relative viewport coordinates with origin in the
  // viewport center
  double normalizedPointX = (normX - viewport[0]) / (viewport[2] - viewport[0]) * 2.0 - 1.0;
  // Calculating the world coordinates offset of the mouse position x-coordinate in relation to the
  // center of the viewport before and after dolling
  double x1 = parallelScale * normalizedPointX * screenSize[0] / screenSize[1];
  double x2 = newParallelScale * normalizedPointX * screenSize[0] / screenSize[1];
  // Calculating the world coordinates displacement along the screen x-axis resulting from dolling
  double xDisplacement = x1 - x2;

  // Converting the mouse position y-coordinate to relative viewport coordinates with origin in the
  // viewport center
  double normalizedPointY = (normY - viewport[1]) / (viewport[3] - viewport[1]) * 2.0 - 1.0;
  // Calculating the world coordinates offset of the mouse position y-coordinate in relation to the
  // center of the viewport before and after dolling
  double y1 = parallelScale * normalizedPointY;
  double y2 = newParallelScale * normalizedPointY;
  // Calculating the world coordinates displacement along the screen y-axis resulting from dolling
  double yDisplacement = y1 - y2;

  // Calculating the camera position and focal point offsets so that it can panned to be centered at
  // the current mouse cursor position

  // Getting the camera frame vectors
  double focalPoint[3];
  camera->GetFocalPoint(focalPoint);
  double cameraPosition[3];
  camera->GetPosition(cameraPosition);
  double viewVector[3];
  vtkMath::Subtract(focalPoint, cameraPosition, viewVector);
  double viewUp[3];
  camera->GetViewUp(viewUp);
  vtkMath::Normalize(viewUp); // Making sure the camera's up vector is normalized
  double rightVector[3];
  vtkMath::Cross(viewVector, viewUp, rightVector);
  vtkMath::Normalize(rightVector);

  // Scaling the right vector in the camera frame by the world coordinates displacement along the
  // screen x-axis
  vtkMath::MultiplyScalar(rightVector, xDisplacement);
  // Moving the camera focal point and position along the camera's right vector by the calculated
  // displacement
  vtkMath::Add(focalPoint, rightVector, focalPoint);
  vtkMath::Add(cameraPosition, rightVector, cameraPosition);

  // Scaling the up vector in the camera frame by the world coordinates displacement along the
  // screen y-axis
  vtkMath::MultiplyScalar(viewUp, yDisplacement);
  // Moving the camera focal point and position along the camera's up vector by the calculated
  // displacement
  vtkMath::Add(focalPoint, viewUp, focalPoint);
  vtkMath::Add(cameraPosition, viewUp, cameraPosition);

  // Apply new camera settings
  camera->SetFocalPoint(focalPoint);
  camera->SetPosition(cameraPosition);
}
