// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkImplicitFrustumRepresentation.h"

#include "vtkActor.h"
#include "vtkAssemblyPath.h"
#include "vtkBoundingBox.h"
#include "vtkBox.h"
#include "vtkCamera.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkCellPicker.h"
#include "vtkConeSource.h"
#include "vtkFrustum.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkImplicitFrustumWidget.h"
#include "vtkImplicitPlaneRepresentation.h"
#include "vtkInteractorObserver.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkOutlineFilter.h"
#include "vtkPickingManager.h"
#include "vtkPlane.h"
#include "vtkPlanes.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkSetGet.h"
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkTubeFilter.h"
#include "vtkType.h"
#include "vtkVector.h"
#include "vtkVectorOperators.h"

#include <algorithm>
#include <limits>
#include <ostream>

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkImplicitFrustumRepresentation);

namespace
{
constexpr int ROTATION_MANIPULATION_FACTOR = 5;
}

//------------------------------------------------------------------------------
vtkImplicitFrustumRepresentation::EdgeHandle::EdgeHandle()
{
  vtkNew<vtkPoints> points;
  points->SetDataTypeToDouble();
  this->PolyData->SetPoints(points);

  vtkNew<vtkCellArray> lines;
  this->PolyData->SetLines(lines);

  this->Tuber->SetInputData(this->PolyData);
  this->Tuber->SetNumberOfSides(12);

  this->Mapper->SetInputConnection(this->Tuber->GetOutputPort());
  this->Actor->SetMapper(this->Mapper);
  // The feature edges or tuber turns on scalar viz - we need it off.
  this->Mapper->ScalarVisibilityOff();
}

//------------------------------------------------------------------------------
vtkImplicitFrustumRepresentation::SphereHandle::SphereHandle()
{
  this->Source->SetThetaResolution(16);
  this->Source->SetPhiResolution(16);
  this->Mapper->SetInputConnection(this->Source->GetOutputPort());
  this->Actor->SetMapper(this->Mapper);
}

//------------------------------------------------------------------------------
vtkImplicitFrustumRepresentation::ArrowHandle::ArrowHandle()
{
  // Create the axis
  this->LineSource->SetResolution(1);
  this->LineMapper->SetInputConnection(this->LineSource->GetOutputPort());
  this->LineActor->SetMapper(this->LineMapper);

  // Create the axis head
  this->HeadSource->SetResolution(12);
  this->HeadSource->SetAngle(25.0);
  this->HeadMapper->SetInputConnection(this->HeadSource->GetOutputPort());
  this->HeadActor->SetMapper(this->HeadMapper);
}

//------------------------------------------------------------------------------
vtkImplicitFrustumRepresentation::EllipseHandle::EllipseHandle()
{
  this->Source->SetClose(true);
  this->Source->SetStartAngle(0);
  this->Source->SetSegmentAngle(360);
  this->Tuber->SetInputConnection(this->Source->GetOutputPort());
  this->Tuber->SetNumberOfSides(12);
  this->Mapper->SetInputConnection(this->Tuber->GetOutputPort());
  this->Actor->SetMapper(this->Mapper);
}

//------------------------------------------------------------------------------
vtkImplicitFrustumRepresentation::vtkImplicitFrustumRepresentation()
{

  vtkNew<vtkTransform> transform;
  transform->Identity();
  this->Frustum->SetTransform(transform);

  this->OrientationTransform->Identity();
  this->OrientationTransform->PostMultiply();

  this->InteractionState = InteractionStateType::Outside;

  // Handle size is in pixels for this widget
  this->HandleSize = 5.0;

  // Build the representation of the widget
  vtkNew<vtkPoints> pts;
  pts->SetDataTypeToDouble();
  this->FrustumPD->SetPoints(pts);

  vtkNew<vtkCellArray> polys;
  this->FrustumPD->SetPolys(polys);

  this->FrustumMapper->SetInputData(this->FrustumPD);
  this->FrustumActor->SetMapper(this->FrustumMapper);

  // Define the point coordinates
  std::array<double, 6> bounds = {
    -0.5,
    0.5,
    -0.5,
    0.5,
    -0.5,
    0.5,
  };

  // Initial creation of the widget, serves to initialize it
  this->PlaceWidget(bounds.data());

  // Manage the picking stuff
  this->Picker->SetTolerance(0.005);
  this->Picker->AddPickList(this->AxisHandle.LineActor);
  this->Picker->AddPickList(this->AxisHandle.HeadActor);
  this->Picker->AddPickList(this->OriginHandle.Actor);
  this->Picker->AddPickList(this->NearPlaneEdgesHandle.Actor);
  this->Picker->AddPickList(this->FarPlaneHorizontalHandle.Actor);
  this->Picker->AddPickList(this->FarPlaneVerticalHandle.Actor);
  this->Picker->AddPickList(this->RollHandle.Actor);
  this->Picker->PickFromListOn();

  this->FrustumPicker->SetTolerance(0.005);
  this->FrustumPicker->AddPickList(this->FrustumActor);
  this->FrustumPicker->PickFromListOn();

  // Set up the initial properties
  // Frustum properties
  this->FrustumProperty->SetAmbient(1.0);
  this->FrustumProperty->SetAmbientColor(1.0, 1.0, 1.0);
  this->FrustumProperty->SetOpacity(0.5);

  this->SelectedFrustumProperty->SetAmbient(1.0);
  this->SelectedFrustumProperty->SetAmbientColor(0.0, 1.0, 0.0);
  this->SelectedFrustumProperty->SetOpacity(0.25);

  // Frustum axis properties
  this->AxisProperty->SetAmbient(1.0);
  this->AxisProperty->SetColor(1, 0, 0);
  this->AxisProperty->SetLineWidth(2);

  this->SelectedAxisProperty->SetAmbient(1.0);
  this->SelectedAxisProperty->SetColor(0, 1, 0);
  this->SelectedAxisProperty->SetLineWidth(2);

  // Origin handle properties
  this->OriginHandleProperty->SetAmbient(1.0);
  this->OriginHandleProperty->SetColor(1, 0, 0);

  this->SelectedOriginHandleProperty->SetAmbient(1.0);
  this->SelectedOriginHandleProperty->SetColor(0, 1, 0);

  // Edge property
  this->EdgeHandleProperty->SetAmbient(1.0);
  this->EdgeHandleProperty->SetColor(1.0, 0.0, 0.0);
  this->SelectedEdgeHandleProperty->SetAmbient(1.0);
  this->SelectedEdgeHandleProperty->SetColor(0.0, 1.0, 0.0);

  // Pass the initial properties to the actors.
  this->FrustumActor->SetProperty(this->FrustumProperty);
  this->AxisHandle.LineActor->SetProperty(this->AxisProperty);
  this->AxisHandle.HeadActor->SetProperty(this->AxisProperty);
  this->OriginHandle.Actor->SetProperty(this->OriginHandleProperty);
  this->FrustumActor->SetProperty(this->FrustumProperty);
  this->FarPlaneHorizontalHandle.Actor->SetProperty(this->EdgeHandleProperty);
  this->FarPlaneVerticalHandle.Actor->SetProperty(this->EdgeHandleProperty);
  this->NearPlaneEdgesHandle.Actor->SetProperty(this->EdgeHandleProperty);
  this->RollHandle.Actor->SetProperty(this->OriginHandleProperty); // TODO
}

//------------------------------------------------------------------------------
vtkImplicitFrustumRepresentation::~vtkImplicitFrustumRepresentation() = default;

//------------------------------------------------------------------------------
int vtkImplicitFrustumRepresentation::ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
  // See if anything has been selected
  vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->Picker);

  // The second picker may need to be called. This is done because the frustum
  // wraps around things that can be picked; thus the frustum is the selection
  // of last resort.
  if (path == nullptr)
  {
    this->FrustumPicker->Pick(X, Y, 0., this->Renderer);
    this->FrustumPicker->GetCellId();
    path = this->FrustumPicker->GetPath();
  }

  if (path == nullptr) // Nothing picked
  {
    this->SetRepresentationState(InteractionStateType::Outside);
    this->InteractionState = InteractionStateType::Outside;
    return this->InteractionState;
  }

  // Something picked, continue
  this->ValidPick = true;

  // Depending on the interaction state (set by the widget) we modify
  // the state of the representation based on what is picked.
  if (this->InteractionState == InteractionStateType::Moving)
  {
    vtkProp* prop = path->GetFirstNode()->GetViewProp();
    if (prop == this->AxisHandle.LineActor || prop == this->AxisHandle.HeadActor)
    {
      this->InteractionState = InteractionStateType::RotatingAxis;
      this->SetRepresentationState(InteractionStateType::RotatingAxis);
    }
    else if (prop == this->OriginHandle.Actor)
    {
      this->InteractionState = InteractionStateType::MovingOrigin;
      this->SetRepresentationState(InteractionStateType::MovingOrigin);
    }
    else if (prop == this->NearPlaneEdgesHandle.Actor)
    {
      this->InteractionState = InteractionStateType::AdjustingNearPlaneDistance;
      this->SetRepresentationState(InteractionStateType::AdjustingNearPlaneDistance);
    }
    else if (prop == this->FarPlaneHorizontalHandle.Actor)
    {
      this->InteractionState = InteractionStateType::AdjustingHorizontalAngle;
      this->SetRepresentationState(InteractionStateType::AdjustingHorizontalAngle);
    }
    else if (prop == this->FarPlaneVerticalHandle.Actor)
    {
      this->InteractionState = InteractionStateType::AdjustingVerticalAngle;
      this->SetRepresentationState(InteractionStateType::AdjustingVerticalAngle);
    }
    else if (prop == this->FrustumActor)
    {
      // Choose rotation axis according to cell id
      auto pickedCellId = this->FrustumPicker->GetCellId();
      if (pickedCellId == static_cast<vtkIdType>(FrustumFace::Bottom) ||
        pickedCellId == static_cast<vtkIdType>(FrustumFace::Top))
      {
        this->InteractionState = InteractionStateType::AdjustingPitch;
        this->SetRepresentationState(InteractionStateType::AdjustingPitch);
      }
      else if (pickedCellId == static_cast<vtkIdType>(FrustumFace::Right) ||
        pickedCellId == static_cast<vtkIdType>(FrustumFace::Left))
      {
        this->InteractionState = InteractionStateType::AdjustingYaw;
        this->SetRepresentationState(InteractionStateType::AdjustingYaw);
      }
    }
    else if (prop == this->RollHandle.Actor)
    {
      this->InteractionState = InteractionStateType::AdjustingRoll;
      this->SetRepresentationState(InteractionStateType::AdjustingRoll);
    }
    else
    {
      this->InteractionState = InteractionStateType::Outside;
      this->SetRepresentationState(InteractionStateType::Outside);
    }
  }

  // We may add a condition to allow the camera to work IO scaling
  else if (this->InteractionState != InteractionStateType::Scaling)
  {
    this->InteractionState = InteractionStateType::Outside;
  }

  return this->InteractionState;
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetRepresentationState(InteractionStateType state)
{
  if (this->RepresentationState == state)
  {
    return;
  }

  state = vtkMath::ClampValue(
    state, InteractionStateType::Outside, InteractionStateType::TranslatingOrigin);

  this->RepresentationState = state;
  this->Modified();

  this->HighlightAxis(false);
  this->HighlightFrustum(false);
  this->HighlightOriginHandle(false);
  this->HighlightFarPlaneHorizontalHandle(false);
  this->HighlightFarPlaneVerticalHandle(false);
  this->HighlighNearPlaneHandle(false);

  switch (state)
  {
    case InteractionStateType::RotatingAxis:
      this->HighlightAxis(true);
      break;

    case InteractionStateType::TranslatingOrigin:
    case InteractionStateType::MovingOrigin:
      this->HighlightOriginHandle(true);
      break;

    case InteractionStateType::AdjustingHorizontalAngle:
      this->HighlightFarPlaneHorizontalHandle(true);
      break;

    case InteractionStateType::AdjustingVerticalAngle:
      this->HighlightFarPlaneVerticalHandle(true);
      break;

    case InteractionStateType::AdjustingNearPlaneDistance:
      this->HighlighNearPlaneHandle(true);
      break;

    case InteractionStateType::Scaling:
      if (this->ScaleEnabled)
      {
        this->HighlightAxis(true);
        this->HighlightFrustum(true);
        this->HighlightOriginHandle(true);
        this->HighlightFarPlaneHorizontalHandle(true);
        this->HighlightFarPlaneVerticalHandle(true);
        this->HighlighNearPlaneHandle(true);
      }
      break;

    default:
      break;
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::StartWidgetInteraction(double e[2])
{
  this->StartEventPosition[0] = e[0];
  this->StartEventPosition[1] = e[1];
  this->StartEventPosition[2] = 0.0;
  this->LastEventPosition = { e[0], e[1], 0.0 };
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::WidgetInteraction(double e[2])
{
  vtkCamera* camera = this->Renderer->GetActiveCamera();
  if (camera == nullptr)
  {
    return;
  }

  vtkVector2d eventPosition(e);

  // Do different things depending on state
  // Calculations everybody does
  // Compute the two points defining the motion vector
  vtkVector3d pickPosition;
  this->Picker->GetPickPosition(pickPosition.GetData());

  vtkVector4d focalPoint;
  vtkInteractorObserver::ComputeWorldToDisplay(
    this->Renderer, pickPosition[0], pickPosition[1], pickPosition[2], focalPoint.GetData());
  double z = focalPoint[2];

  // Note: vtkVector4d::GetXYZ() methods would make this cleaner
  vtkVector4d prevPickPoint4d;
  vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer, this->LastEventPosition[0],
    this->LastEventPosition[1], z, prevPickPoint4d.GetData());
  vtkVector3d prevPickPoint(prevPickPoint4d.GetX(), prevPickPoint4d.GetY(), prevPickPoint4d.GetZ());

  vtkVector4d pickPoint4d;
  vtkInteractorObserver::ComputeDisplayToWorld(
    this->Renderer, e[0], e[1], z, pickPoint4d.GetData());
  vtkVector3d pickPoint(pickPoint4d.GetX(), pickPoint4d.GetY(), pickPoint4d.GetZ());

  // Process the motion
  switch (this->InteractionState)
  {
    case InteractionStateType::MovingOrigin:
      this->TranslateOrigin(prevPickPoint, pickPoint);
      break;

    case InteractionStateType::TranslatingOrigin:
      this->TranslateOriginOnAxis(prevPickPoint, pickPoint);
      break;

    case InteractionStateType::Scaling:
      if (this->ScaleEnabled)
      {
        this->Scale(prevPickPoint, pickPoint, e[0], e[1]);
      }
      break;

    case InteractionStateType::RotatingAxis:
      // vtkVector3d vpn;
      // camera->GetViewPlaneNormal(vpn.GetData());
      // this->Rotate(e[0], e[1], prevPickPoint, pickPoint, vpn);
      break;

    case InteractionStateType::AdjustingHorizontalAngle:
      this->AdjustHorizontalAngle(eventPosition, prevPickPoint, pickPoint);
      break;

    case InteractionStateType::AdjustingVerticalAngle:
      this->AdjustVerticalAngle(eventPosition, prevPickPoint, pickPoint);
      break;

    case InteractionStateType::AdjustingNearPlaneDistance:
      this->AdjustNearPlaneDistance(eventPosition, prevPickPoint, pickPoint);
      break;

    case InteractionStateType::AdjustingYaw:
      // this->AdjustYaw(prevPickPoint, pickPoint);
      this->RotateOrientation(prevPickPoint, pickPoint, vtkVector3d(0, 0, 1));
      break;

    case InteractionStateType::AdjustingPitch:
      // this->AdjustPitch(prevPickPoint, pickPoint);
      this->RotateOrientation(prevPickPoint, pickPoint, vtkVector3d(1, 0, 0));

      break;

    case InteractionStateType::AdjustingRoll:
      // this->AdjustRoll(prevPickPoint, pickPoint);
      this->RotateOrientation(prevPickPoint, pickPoint, vtkVector3d(0, 1, 0));

      break;
  }

  this->LastEventPosition = { e[0], e[1], 0.0 };

  this->BuildRepresentation();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::EndWidgetInteraction(double vtkNotUsed(e)[2])
{
  this->SetRepresentationState(InteractionStateType::Outside);
}

//------------------------------------------------------------------------------
double* vtkImplicitFrustumRepresentation::GetBounds()
{
  this->BuildRepresentation();
  this->BoundingBox->AddBounds(this->FrustumActor->GetBounds());
  this->BoundingBox->AddBounds(this->FarPlaneHorizontalHandle.Actor->GetBounds());
  this->BoundingBox->AddBounds(this->FarPlaneVerticalHandle.Actor->GetBounds());
  this->BoundingBox->AddBounds(this->NearPlaneEdgesHandle.Actor->GetBounds());
  this->BoundingBox->AddBounds(this->AxisHandle.LineActor->GetBounds());
  this->BoundingBox->AddBounds(this->AxisHandle.HeadActor->GetBounds());
  this->BoundingBox->AddBounds(this->OriginHandle.Actor->GetBounds());
  this->BoundingBox->AddBounds(this->RollHandle.Actor->GetBounds());
  this->BoundingBox->AddBounds(this->ViewUpHandle.LineActor->GetBounds());
  this->BoundingBox->AddBounds(this->ViewUpHandle.HeadActor->GetBounds());

  return this->BoundingBox->GetBounds();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetActors(vtkPropCollection* pc)
{
  this->FarPlaneHorizontalHandle.Actor->GetActors(pc);
  this->FarPlaneVerticalHandle.Actor->GetActors(pc);
  this->NearPlaneEdgesHandle.Actor->GetActors(pc);
  this->AxisHandle.HeadActor->GetActors(pc);
  this->AxisHandle.LineActor->GetActors(pc);
  this->OriginHandle.Actor->GetActors(pc);
  this->RollHandle.Actor->GetActors(pc);

  this->ViewUpHandle.HeadActor->GetActors(pc);
  this->ViewUpHandle.LineActor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::ReleaseGraphicsResources(vtkWindow* w)
{
  this->FrustumActor->ReleaseGraphicsResources(w);
  this->FarPlaneHorizontalHandle.Actor->ReleaseGraphicsResources(w);
  this->FarPlaneVerticalHandle.Actor->ReleaseGraphicsResources(w);
  this->NearPlaneEdgesHandle.Actor->ReleaseGraphicsResources(w);
  this->AxisHandle.LineActor->ReleaseGraphicsResources(w);
  this->AxisHandle.HeadActor->ReleaseGraphicsResources(w);
  this->OriginHandle.Actor->ReleaseGraphicsResources(w);
  this->RollHandle.Actor->ReleaseGraphicsResources(w);
  this->ViewUpHandle.LineActor->ReleaseGraphicsResources(w);
  this->ViewUpHandle.HeadActor->ReleaseGraphicsResources(w);
}

//------------------------------------------------------------------------------
int vtkImplicitFrustumRepresentation::RenderOpaqueGeometry(vtkViewport* v)
{
  int count = 0;
  this->BuildRepresentation();
  count += this->FarPlaneHorizontalHandle.Actor->RenderOpaqueGeometry(v);
  count += this->FarPlaneVerticalHandle.Actor->RenderOpaqueGeometry(v);
  count += this->NearPlaneEdgesHandle.Actor->RenderOpaqueGeometry(v);
  count += this->AxisHandle.LineActor->RenderOpaqueGeometry(v);
  count += this->AxisHandle.HeadActor->RenderOpaqueGeometry(v);
  count += this->OriginHandle.Actor->RenderOpaqueGeometry(v);
  count += this->RollHandle.Actor->RenderOpaqueGeometry(v);
  count += this->ViewUpHandle.LineActor->RenderOpaqueGeometry(v);
  count += this->ViewUpHandle.HeadActor->RenderOpaqueGeometry(v);
  if (this->DrawFrustum)
  {
    count += this->FrustumActor->RenderOpaqueGeometry(v);
  }

  return count;
}

//------------------------------------------------------------------------------
int vtkImplicitFrustumRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport* v)
{
  int count = 0;
  this->BuildRepresentation();
  count += this->FarPlaneHorizontalHandle.Actor->RenderTranslucentPolygonalGeometry(v);
  count += this->FarPlaneVerticalHandle.Actor->RenderTranslucentPolygonalGeometry(v);
  count += this->NearPlaneEdgesHandle.Actor->RenderTranslucentPolygonalGeometry(v);
  count += this->AxisHandle.LineActor->RenderTranslucentPolygonalGeometry(v);
  count += this->AxisHandle.HeadActor->RenderTranslucentPolygonalGeometry(v);
  count += this->OriginHandle.Actor->RenderTranslucentPolygonalGeometry(v);
  count += this->RollHandle.Actor->RenderTranslucentPolygonalGeometry(v);
  count += this->ViewUpHandle.LineActor->RenderTranslucentPolygonalGeometry(v);
  count += this->ViewUpHandle.HeadActor->RenderTranslucentPolygonalGeometry(v);
  if (this->DrawFrustum)
  {
    count += this->FrustumActor->RenderTranslucentPolygonalGeometry(v);
  }

  return count;
}

//------------------------------------------------------------------------------
vtkTypeBool vtkImplicitFrustumRepresentation::HasTranslucentPolygonalGeometry()
{
  bool result = false;
  result |= this->FarPlaneHorizontalHandle.Actor->HasTranslucentPolygonalGeometry();
  result |= this->FarPlaneVerticalHandle.Actor->HasTranslucentPolygonalGeometry();
  result |= this->NearPlaneEdgesHandle.Actor->HasTranslucentPolygonalGeometry();
  result |= this->AxisHandle.LineActor->HasTranslucentPolygonalGeometry();
  result |= this->AxisHandle.HeadActor->HasTranslucentPolygonalGeometry();
  result |= this->OriginHandle.Actor->HasTranslucentPolygonalGeometry();
  result |= this->RollHandle.Actor->HasTranslucentPolygonalGeometry();
  result |= this->ViewUpHandle.LineActor->HasTranslucentPolygonalGeometry();
  result |= this->ViewUpHandle.HeadActor->HasTranslucentPolygonalGeometry();

  if (this->DrawFrustum)
  {
    result |= this->FrustumActor->HasTranslucentPolygonalGeometry();
  }

  return result;
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Resolution: " << this->Resolution << std::endl;

  os << indent << "Axis Property: " << this->AxisProperty << std::endl;
  os << indent << "Selected Axis Property: " << this->SelectedAxisProperty << std::endl;
  os << indent << "Frustum Property: " << this->FrustumProperty << std::endl;
  os << indent << "Selected Frustum Property: " << this->SelectedFrustumProperty << std::endl;
  os << indent << "Edges Property: " << this->EdgeHandleProperty << std::endl;

  os << indent << "Along X Axis: " << (this->AlongXAxis ? "On" : "Off") << std::endl;
  os << indent << "Along Y Axis: " << (this->AlongYAxis ? "On" : "Off") << std::endl;
  os << indent << "ALong Z Axis: " << (this->AlongZAxis ? "On" : "Off") << std::endl;

  os << indent << "Widget Bounds: " << this->WidgetBounds << std::endl;

  os << indent << "Constrain to Widget Bounds: " << (this->ConstrainToWidgetBounds ? "On" : "Off")
     << std::endl;
  os << indent << "Scale Enabled: " << (this->ScaleEnabled ? "On" : "Off") << std::endl;
  os << indent << "Draw Frustum: " << (this->DrawFrustum ? "On" : "Off") << std::endl;
  os << indent << "Bump Distance: " << this->BumpDistance << std::endl;

  os << indent << "Representation State: ";
  switch (this->RepresentationState)
  {
    case Outside:
      os << "Outside" << std::endl;
      break;
    case Moving:
      os << "Moving" << std::endl;
      break;
    case MovingOrigin:
      os << "MovingOrigin" << std::endl;
      break;
    case RotatingAxis:
      os << "RotatingAxis" << std::endl;
      break;
    case Scaling:
      os << "Scaling" << std::endl;
      break;
    case TranslatingOrigin:
      os << "TranslatingOrigin" << std::endl;
      break;
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::HighlightAxis(bool highlight)
{
  if (highlight)
  {
    this->AxisHandle.LineActor->SetProperty(this->SelectedAxisProperty);
    this->AxisHandle.HeadActor->SetProperty(this->SelectedAxisProperty);
  }
  else
  {
    this->AxisHandle.LineActor->SetProperty(this->AxisProperty);
    this->AxisHandle.HeadActor->SetProperty(this->AxisProperty);
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::HighlightOriginHandle(bool highlight)
{
  if (highlight)
  {
    this->OriginHandle.Actor->SetProperty(this->SelectedOriginHandleProperty);
  }
  else
  {
    this->OriginHandle.Actor->SetProperty(this->OriginHandleProperty);
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::HighlightFrustum(bool highlight)
{
  if (highlight)
  {
    this->FrustumActor->SetProperty(this->SelectedFrustumProperty);
    this->FarPlaneHorizontalHandle.Actor->SetProperty(this->SelectedFrustumProperty);
    this->FarPlaneVerticalHandle.Actor->SetProperty(this->SelectedFrustumProperty);
    this->NearPlaneEdgesHandle.Actor->SetProperty(this->SelectedFrustumProperty);
  }
  else
  {
    this->FrustumActor->SetProperty(this->FrustumProperty);
    this->FarPlaneHorizontalHandle.Actor->SetProperty(this->EdgeHandleProperty);
    this->FarPlaneVerticalHandle.Actor->SetProperty(this->EdgeHandleProperty);
    this->NearPlaneEdgesHandle.Actor->SetProperty(this->EdgeHandleProperty);
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::HighlightFarPlaneVerticalHandle(bool highlight)
{
  if (highlight)
  {
    this->FarPlaneVerticalHandle.Actor->SetProperty(this->SelectedEdgeHandleProperty);
  }
  else
  {
    this->FarPlaneVerticalHandle.Actor->SetProperty(this->EdgeHandleProperty);
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::HighlightFarPlaneHorizontalHandle(bool highlight)
{
  if (highlight)
  {
    this->FarPlaneHorizontalHandle.Actor->SetProperty(this->SelectedEdgeHandleProperty);
  }
  else
  {
    this->FarPlaneHorizontalHandle.Actor->SetProperty(this->EdgeHandleProperty);
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::HighlighNearPlaneHandle(bool highlight)
{
  if (highlight)
  {
    this->NearPlaneEdgesHandle.Actor->SetProperty(this->SelectedEdgeHandleProperty);
  }
  else
  {
    this->NearPlaneEdgesHandle.Actor->SetProperty(this->EdgeHandleProperty);
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::TranslateOrigin(const vtkVector3d& p1, const vtkVector3d& p2)
{
  vtkCamera* camera = this->Renderer->GetActiveCamera();
  if (camera == nullptr)
  {
    return;
  }

  // Get the motion vector
  vtkVector3d v = { 0, 0, 0 };

  if (!this->IsTranslationConstrained())
  {
    v = p2 - p1;
  }
  else
  {
    if (this->TranslationAxis < Axis::XAxis || this->TranslationAxis > Axis::ZAxis)
    {
      vtkWarningMacro("this->TranslationAxis out of bounds");
      return;
    }
    v[this->TranslationAxis] = p2[this->TranslationAxis] - p1[this->TranslationAxis];
  }

  // Translate the current origin

  vtkVector3d newOrigin = this->Origin + v;

  // Project back onto plane orthogonal to camera
  vtkVector3d vpn;
  camera->GetViewPlaneNormal(vpn.GetData());

  vtkPlane::ProjectPoint(
    newOrigin.GetData(), this->Origin.GetData(), vpn.GetData(), newOrigin.GetData());

  this->SetOrigin(newOrigin.GetData());
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::TranslateOriginOnAxis(
  const vtkVector3d& p1, const vtkVector3d& p2)
{
  // vtkVector3d v = p2 - p1;

  // // Add to the current point, project back down onto plane
  // vtkVector3d origin(this->Frustum->GetOrigin());
  // vtkVector3d axis(this->ForwardAxis);
  // vtkVector3d newOrigin = origin + v;

  // // Normalize the axis vector
  // axis.Normalize();

  // // Project the point on the axis vector
  // vtkVector3d u = newOrigin - origin;
  // newOrigin = origin + (axis * axis.Dot(u));
  // this->Frustum->SetOrigin(newOrigin.GetData());
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::Scale(
  const vtkVector3d& p1, const vtkVector3d& p2, double vtkNotUsed(X), double Y)
{
  // Get the motion vector
  // vtkVector3d v = p2 - p1;

  // vtkVector3d frustumOrigin(this->Frustum->GetOrigin());

  // // Compute the scale factor
  // double diagonal = this->Outline->GetOutput()->GetLength();
  // if (diagonal == 0.)
  // {
  //   return;
  // }
  // double sf = v.Norm() / diagonal;
  // if (Y > this->LastEventPosition[1])
  // {
  //   sf = 1.0 + sf;
  // }
  // else
  // {
  //   sf = 1.0 - sf;
  // }

  // vtkNew<vtkTransform> transform;
  // transform->Identity();
  // transform->Translate(frustumOrigin.GetData());
  // transform->Scale(sf, sf, sf);
  // transform->Translate((-frustumOrigin).GetData());

  // vtkVector3d boxCenter(this->Box->GetCenter());
  // vtkVector3d spacing(this->Box->GetSpacing());
  // vtkVector3d p = boxCenter + spacing;

  // vtkVector3d oNew, pNew;
  // transform->TransformPoint(boxCenter.GetData(), oNew.GetData());
  // transform->TransformPoint(p.GetData(), pNew.GetData());

  // this->Box->SetOrigin(oNew.GetData());
  // this->Box->SetSpacing((pNew - oNew).GetData());
  // this->Box->GetBounds(this->WidgetBounds.GetData());
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::AdjustHorizontalAngle(
  const vtkVector2d& eventPosition, const vtkVector3d& p1, const vtkVector3d& p2)
{
  if (eventPosition[1] == this->LastEventPosition[1])
  {
    return;
  }

  double currentAngle = this->Frustum->GetHorizontalAngle();
  vtkVector3d v = p2 - p1;

  const double angleManipulationFactor = 5.0;
  double deltaAngle = std::sqrt(v.Dot(v)) * angleManipulationFactor;

  if (eventPosition[1] < this->LastEventPosition[1])
  {
    deltaAngle = -deltaAngle;
  }

  this->Frustum->SetHorizontalAngle(currentAngle + deltaAngle);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::AdjustVerticalAngle(
  const vtkVector2d& eventPosition, const vtkVector3d& p1, const vtkVector3d& p2)
{
  if (eventPosition[1] == this->LastEventPosition[1])
  {
    return;
  }

  double currentAngle = this->Frustum->GetVerticalAngle();
  vtkVector3d v = p2 - p1;

  const double angleManipulationFactor = 5.0;
  double deltaAngle = std::sqrt(v.Dot(v)) * angleManipulationFactor;

  if (eventPosition[1] < this->LastEventPosition[1])
  {
    deltaAngle = -deltaAngle;
  }

  this->Frustum->SetVerticalAngle(currentAngle + deltaAngle);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::AdjustNearPlaneDistance(
  const vtkVector2d& eventPosition, const vtkVector3d& p1, const vtkVector3d& p2)
{
  if (eventPosition[1] == this->LastEventPosition[1])
  {
    return;
  }

  double currentDistance = this->Frustum->GetNearPlaneDistance();
  vtkVector3d v = p2 - p1;

  // TODO: Change sign according to v . axis
  double deltaDistance = v.Norm();

  if (eventPosition[1] < this->LastEventPosition[1])
  {
    deltaDistance = -deltaDistance;
  }

  this->Frustum->SetNearPlaneDistance(currentDistance + deltaDistance);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetInteractionColor(double r, double g, double b)
{
  this->SelectedAxisProperty->SetColor(r, g, b);
  this->SelectedFrustumProperty->SetAmbientColor(r, g, b);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetHandleColor(double r, double g, double b)
{
  this->AxisProperty->SetColor(r, g, b);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetForegroundColor(double r, double g, double b)
{
  this->FrustumProperty->SetAmbientColor(r, g, b);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::PlaceWidget(double bds[6])
{
  vtkVector<double, 6> bounds;
  vtkVector3d center;
  this->AdjustBounds(bds, bounds.GetData(), center.GetData());

  // TODO: handle the up axis here
  this->AxisHandle.LineSource->SetPoint1(this->GetOrigin());
  if (this->AlongXAxis)
  {
    this->SetForwardAxis(1, 0, 0);
    this->AxisHandle.LineSource->SetPoint2(1, 0, 0);
  }
  else if (this->AlongZAxis)
  {
    this->SetForwardAxis(0, 0, 1);
    this->AxisHandle.LineSource->SetPoint2(0, 0, 1);
  }
  else // default or x-normal
  {
    this->SetForwardAxis(0, 1, 0);
    this->AxisHandle.LineSource->SetPoint2(0, 1, 0);
  }

  for (int i = 0; i < 6; i++)
  {
    this->InitialBounds[i] = bounds[i];
    this->WidgetBounds[i] = bounds[i];
  }

  this->OrientationTransform->Identity();

  vtkBoundingBox bbox(bounds.GetData());
  this->InitialLength = bbox.GetDiagonalLength();

  this->ValidPick = true; // since we have positioned the widget successfully
  this->BuildRepresentation();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetOrigin(const vtkVector3d& xyz)
{
  if (xyz != this->Origin)
  {
    this->Origin = xyz;
    this->Modified();
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetOrigin(double x, double y, double z)
{
  this->SetOrigin(vtkVector3d(x, y, z));
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetOrigin(double xyz[3])
{
  this->SetOrigin(vtkVector3d(xyz));
}

//------------------------------------------------------------------------------
double* vtkImplicitFrustumRepresentation::GetOrigin()
{
  return this->Origin.GetData();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetOrigin(double xyz[3]) const
{
  xyz[0] = this->Origin[0];
  xyz[1] = this->Origin[1];
  xyz[2] = this->Origin[2];
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetForwardAxis(const vtkVector3d& axis)
{
  vtkVector3d n = axis.Normalized();
  if (n != this->ForwardAxis)
  {
    this->ForwardAxis = n;
    this->Modified();
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetForwardAxis(double x, double y, double z)
{
  this->SetForwardAxis(vtkVector3d(x, y, z));
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetForwardAxis(double n[3])
{
  this->SetForwardAxis(vtkVector3d(n));
}

//------------------------------------------------------------------------------
double* vtkImplicitFrustumRepresentation::GetForwardAxis()
{
  return this->ForwardAxis.GetData();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetForwardAxis(double xyz[3]) const
{
  xyz[0] = this->ForwardAxis[0];
  xyz[1] = this->ForwardAxis[1];
  xyz[2] = this->ForwardAxis[2];
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetUpAxis(const vtkVector3d& axis)
{
  vtkVector3d n = axis.Normalized();
  if (n != this->UpAxis)
  {
    this->UpAxis = n;
    this->Modified();
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetUpAxis(double x, double y, double z)
{
  this->SetUpAxis(vtkVector3d(x, y, z));
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetUpAxis(double n[3])
{
  this->SetUpAxis(vtkVector3d(n));
}

//------------------------------------------------------------------------------
double* vtkImplicitFrustumRepresentation::GetUpAxis()
{
  return this->UpAxis.GetData();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetUpAxis(double xyz[3]) const
{
  xyz[0] = this->UpAxis[0];
  xyz[1] = this->UpAxis[1];
  xyz[2] = this->UpAxis[2];
}

void vtkImplicitFrustumRepresentation::RotateOrientation(
  const vtkVector3d& prevPickPoint, const vtkVector3d& pickPoint, const vtkVector3d& axis)
{
  const vtkVector3d centeredP1 = prevPickPoint - this->Origin;
  const vtkVector3d centeredP2 = pickPoint - this->Origin;

  vtkVector3d rotationAxis(this->OrientationTransform->TransformVector(axis.GetData()));
  double rotationAngle = vtkMath::SignedAngleBetweenVectors(
    centeredP1.GetData(), centeredP2.GetData(), rotationAxis.GetData());

  this->OrientationTransform->RotateWXYZ(
    vtkMath::DegreesFromRadians(rotationAngle), rotationAxis.GetData());

  vtkNew<vtkTransform> transform;
  transform->Identity();
  transform->Concatenate(this->OrientationTransform);
  // transform->Translate(this->Origin.GetData());
  this->Frustum->SetTransform(transform);

  // Just debugging stuff
  vtkVector3d up(0, 0, -1);
  this->UpAxis = vtkVector3d(transform->TransformPoint(up.GetData()));

  vtkVector3d forward(0, 1, 0);
  this->ForwardAxis = vtkVector3d(transform->TransformPoint(forward.GetData()));
}

//------------------------------------------------------------------------------
double vtkImplicitFrustumRepresentation::GetHorizontalAngle() const
{
  return this->Frustum->GetHorizontalAngle();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetHorizontalAngle(double angle)
{
  this->Frustum->SetHorizontalAngle(angle);
}

//------------------------------------------------------------------------------
double vtkImplicitFrustumRepresentation::GetVerticalAngle() const
{
  return this->Frustum->GetVerticalAngle();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetVerticalAngle(double angle)
{
  this->Frustum->SetVerticalAngle(angle);
}

//------------------------------------------------------------------------------
double vtkImplicitFrustumRepresentation::GetNearPlaneDistance() const
{
  return this->Frustum->GetNearPlaneDistance();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetNearPlaneDistance(double distance)
{
  this->Frustum->SetNearPlaneDistance(distance);
}

//------------------------------------------------------------------------------
double* vtkImplicitFrustumRepresentation::GetWidgetBounds()
{
  return this->WidgetBounds.GetData();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetDrawFrustum(bool drawFrustum)
{
  if (drawFrustum == this->DrawFrustum)
  {
    return;
  }

  this->Modified();
  this->DrawFrustum = drawFrustum;
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetAlongXAxis(bool var)
{
  if (this->AlongXAxis != var)
  {
    this->AlongXAxis = var;
    this->Modified();
  }
  if (var)
  {
    this->AlongYAxisOff();
    this->AlongZAxisOff();
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetAlongYAxis(bool var)
{
  if (this->AlongYAxis != var)
  {
    this->AlongYAxis = var;
    this->Modified();
  }
  if (var)
  {
    this->AlongXAxisOff();
    this->AlongZAxisOff();
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetAlongZAxis(bool var)
{
  if (this->AlongZAxis != var)
  {
    this->AlongZAxis = var;
    this->Modified();
  }
  if (var)
  {
    this->AlongXAxisOff();
    this->AlongYAxisOff();
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetPolyData(vtkPolyData* pd)
{
  pd->ShallowCopy(this->FrustumPD);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::UpdatePlacement()
{
  this->BuildRepresentation();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::BuildRepresentation()
{
  if (!this->Renderer || !this->Renderer->GetRenderWindow())
  {
    return;
  }

  if (this->GetMTime() > this->BuildTime || this->Frustum->GetMTime() > this->BuildTime ||
    this->Renderer->GetRenderWindow()->GetMTime() > this->BuildTime)
  {

    vtkInformation* info = this->GetPropertyKeys();
    this->FrustumActor->SetPropertyKeys(info);
    this->FarPlaneHorizontalHandle.Actor->SetPropertyKeys(info);
    this->FarPlaneVerticalHandle.Actor->SetPropertyKeys(info);
    this->NearPlaneEdgesHandle.Actor->SetPropertyKeys(info);
    this->AxisHandle.LineActor->SetPropertyKeys(info);
    this->AxisHandle.HeadActor->SetPropertyKeys(info);
    this->OriginHandle.Actor->SetPropertyKeys(info);

    vtkVector3d origin(this->GetOrigin());
    vtkVector3d axis(this->GetForwardAxis());

    // Update the adjusted origin
    this->SetOrigin(origin.GetData());

    // Setup the forward and up axis handles
    double d = 10;
    vtkVector3d p2 = origin + this->ForwardAxis * 0.3 * d;
    this->AxisHandle.LineSource->SetPoint1(origin.GetData());
    this->AxisHandle.LineSource->SetPoint2(p2.GetData());
    this->AxisHandle.HeadSource->SetCenter(p2.GetData());
    this->AxisHandle.HeadSource->SetDirection(axis.GetData());

    vtkVector3d upPoint = origin + this->UpAxis * 0.3 * d;
    this->ViewUpHandle.LineSource->SetPoint1(origin.GetData());
    this->ViewUpHandle.LineSource->SetPoint2(upPoint.GetData());
    this->AxisHandle.HeadSource->SetCenter(upPoint.GetData());
    this->AxisHandle.HeadSource->SetDirection(this->UpAxis.GetData());

    // Set up the position handle
    this->OriginHandle.Source->SetCenter(origin.GetData());

    // Place the roll control
    this->RollHandle.Source->SetCenter(origin.GetData());
    this->RollHandle.Source->SetNormal(this->GetForwardAxis());
    // TODO: Set the size of the roll handle

    // Construct frustum
    this->BuildFrustum();

    this->SizeHandles();
    this->BuildTime.Modified();
  }
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SizeHandles()
{
  double radius = this->SizeHandlesInPixels(1.5, this->OriginHandle.Source->GetCenter());

  this->AxisHandle.HeadSource->SetHeight(2.0 * radius);
  this->AxisHandle.HeadSource->SetRadius(radius);

  this->ViewUpHandle.HeadSource->SetHeight(2 * radius);
  this->ViewUpHandle.HeadSource->SetRadius(radius);

  this->OriginHandle.Source->SetRadius(radius);

  this->FarPlaneHorizontalHandle.Tuber->SetRadius(0.25 * radius);
  this->FarPlaneVerticalHandle.Tuber->SetRadius(0.25 * radius);
  this->NearPlaneEdgesHandle.Tuber->SetRadius(0.25 * radius);
  this->RollHandle.Tuber->SetRadius(0.25 * radius);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::BuildFrustum()
{
  const vtkVector3d& forwardAxis = this->ForwardAxis;
  const vtkVector3d& upAxis = this->UpAxis;
  const vtkVector3d& origin = this->Origin;
  const double height = 10; // TODO

  // Generate frustum polydata
  this->FrustumPD->Reset();

  double verticalAngle = vtkMath::RadiansFromDegrees(this->Frustum->GetVerticalAngle());
  double horizontalAngle = vtkMath::RadiansFromDegrees(this->Frustum->GetHorizontalAngle());

  // TODO: Make sure we're drawing right
  double sinVertical = std::sin(verticalAngle);
  double cosVertical = std::cos(verticalAngle);
  double sinHorizontal = std::sin(horizontalAngle);
  double cosHorizontal = std::cos(horizontalAngle);

  std::array<vtkVector3d, 4> edgeDirections;
  edgeDirections[1] = vtkVector3d(cosHorizontal, 1, cosVertical).Normalized();   // bottom-right
  edgeDirections[0] = vtkVector3d(-cosHorizontal, 1, cosVertical).Normalized();  // bottom-left
  edgeDirections[3] = vtkVector3d(-cosHorizontal, 1, -cosVertical).Normalized(); // top-left
  edgeDirections[2] = vtkVector3d(cosHorizontal, 1, -cosVertical).Normalized();  // top-right

  vtkNew<vtkIdList> nearPlanePointIndices;
  nearPlanePointIndices->Allocate(4);
  vtkNew<vtkIdList> farPlanePointIndices;
  farPlanePointIndices->Allocate(4);

  // Annulus base points
  auto transform = this->Frustum->GetTransform();

  vtkPoints* frustumPoints = this->FrustumPD->GetPoints();
  for (const vtkVector3d& direction : edgeDirections)
  {
    vtkVector3d nearPoint = origin + direction * this->Frustum->GetNearPlaneDistance();
    vtkVector3d farPoint = origin + direction * height;

    transform->TransformPoint(nearPoint.GetData(), nearPoint.GetData());
    transform->TransformPoint(farPoint.GetData(), farPoint.GetData());

    vtkIdType nearPointIdx = frustumPoints->InsertNextPoint(nearPoint.GetData());
    vtkIdType farPointIdx = frustumPoints->InsertNextPoint(farPoint.GetData());

    // Register point indices to the plane they belong to create handles later on
    nearPlanePointIndices->InsertNextId(nearPointIdx);
    farPlanePointIndices->InsertNextId(farPointIdx);
  }

  // Create frustum planes
  vtkCellArray* polys = this->FrustumPD->GetPolys();
  polys->InsertNextCell({ 0, 2, 4, 6 }); // Near
  polys->InsertNextCell({ 0, 1, 3, 2 }); // Bottom
  polys->InsertNextCell({ 2, 3, 5, 4 }); // Right
  polys->InsertNextCell({ 4, 5, 7, 6 }); // Top
  polys->InsertNextCell({ 6, 7, 1, 0 }); // Left

  this->FrustumPD->Modified();

  // Create edges handles
  this->NearPlaneEdgesHandle.PolyData->Reset();
  this->FarPlaneHorizontalHandle.PolyData->Reset();
  this->FarPlaneVerticalHandle.PolyData->Reset();

  // Near plane handle
  auto nearPlanePoints = this->NearPlaneEdgesHandle.PolyData->GetPoints();
  frustumPoints->GetPoints(nearPlanePointIndices, nearPlanePoints);

  auto nearPlaneLines = this->NearPlaneEdgesHandle.PolyData->GetLines();
  nearPlaneLines->InsertNextCell({ 0, 1 });
  nearPlaneLines->InsertNextCell({ 1, 2 });
  nearPlaneLines->InsertNextCell({ 2, 3 });
  nearPlaneLines->InsertNextCell({ 3, 0 });

  this->NearPlaneEdgesHandle.PolyData->Modified();

  // Far plane handles
  auto farPlaneHorizontalPoints = this->FarPlaneHorizontalHandle.PolyData->GetPoints();
  frustumPoints->GetPoints(farPlanePointIndices, farPlaneHorizontalPoints);

  auto farPlaneHorizontalLines = this->FarPlaneHorizontalHandle.PolyData->GetLines();
  farPlaneHorizontalLines->InsertNextCell({ 0, 1 });
  farPlaneHorizontalLines->InsertNextCell({ 2, 3 });

  this->FarPlaneHorizontalHandle.PolyData->Modified();

  auto farPlaneVerticalPoints = this->FarPlaneVerticalHandle.PolyData->GetPoints();
  frustumPoints->GetPoints(farPlanePointIndices, farPlaneVerticalPoints);

  auto farPlaneVerticalLines = this->FarPlaneVerticalHandle.PolyData->GetLines();
  farPlaneVerticalLines->InsertNextCell({ 1, 2 });
  farPlaneVerticalLines->InsertNextCell({ 3, 0 });

  this->FarPlaneVerticalHandle.PolyData->Modified();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::RegisterPickers()
{
  vtkPickingManager* pm = this->GetPickingManager();
  if (pm == nullptr)
  {
    return;
  }
  pm->AddPicker(this->Picker, this);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetFrustum(vtkFrustum* frustum) const
{
  if (frustum == nullptr)
  {
    return;
  }

  frustum->SetTransform(frustum->GetTransform());
  frustum->SetHorizontalAngle(this->Frustum->GetHorizontalAngle());
  frustum->SetVerticalAngle(this->Frustum->GetVerticalAngle());
  frustum->SetNearPlaneDistance(this->Frustum->GetNearPlaneDistance());
}

VTK_ABI_NAMESPACE_END
