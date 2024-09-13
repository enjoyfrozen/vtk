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
void vtkImplicitFrustumRepresentation::ArrowHandle::Update(
  const vtkVector3d& origin, const vtkVector3d& direction, double length)
{
  vtkVector3d p2 = origin + direction * length;
  this->LineSource->SetPoint1(origin.GetData());
  this->LineSource->SetPoint2(p2.GetData());
  this->HeadSource->SetCenter(p2.GetData());
  this->HeadSource->SetDirection(direction.GetData());
}

void vtkImplicitFrustumRepresentation::ArrowHandle::SizeHandle(double radius)
{
  this->HeadSource->SetHeight(2.0 * radius);
  this->HeadSource->SetRadius(radius);
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

  // Orientation transform is in post multiply so we can edit it as we go
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

  // Initial creation of the widget, serves to initialize it
  std::array<double, 6> bounds = { -0.5, 0.5, -0.5, 0.5, -0.5, 0.5 };
  this->PlaceWidget(bounds.data());

  // Manage the picking stuff
  this->Picker->SetTolerance(0.005);
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
  this->OriginHandle.Actor->SetProperty(this->OriginHandleProperty);
  this->FrustumActor->SetProperty(this->FrustumProperty);
  this->FarPlaneHorizontalHandle.Actor->SetProperty(this->EdgeHandleProperty);
  this->FarPlaneVerticalHandle.Actor->SetProperty(this->EdgeHandleProperty);
  this->NearPlaneEdgesHandle.Actor->SetProperty(this->EdgeHandleProperty);
  this->RollHandle.Actor->SetProperty(this->EdgeHandleProperty);
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
    if (prop == this->OriginHandle.Actor)
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

  this->HighlightFrustum(false);
  this->HighlightOriginHandle(false);
  this->HighlightFarPlaneHorizontalHandle(false);
  this->HighlightFarPlaneVerticalHandle(false);
  this->HighlighNearPlaneHandle(false);
  this->HighlightRollHandle(false);

  switch (state)
  {
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

    case InteractionStateType::AdjustingRoll:
      this->HighlightRollHandle(true);
      break;

    case InteractionStateType::Scaling:
      if (this->ScaleEnabled)
      {
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
      this->Rotate(prevPickPoint, pickPoint, vtkVector3d(0, 0, 1));
      break;

    case InteractionStateType::AdjustingPitch:
      this->Rotate(prevPickPoint, pickPoint, vtkVector3d(1, 0, 0));
      break;

    case InteractionStateType::AdjustingRoll:
      this->Rotate(prevPickPoint, pickPoint, vtkVector3d(0, 1, 0));
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
  this->BoundingBox->AddBounds(this->OriginHandle.Actor->GetBounds());
  this->BoundingBox->AddBounds(this->RollHandle.Actor->GetBounds());

  return this->BoundingBox->GetBounds();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetActors(vtkPropCollection* pc)
{
  this->FarPlaneHorizontalHandle.Actor->GetActors(pc);
  this->FarPlaneVerticalHandle.Actor->GetActors(pc);
  this->NearPlaneEdgesHandle.Actor->GetActors(pc);
  this->OriginHandle.Actor->GetActors(pc);
  this->RollHandle.Actor->GetActors(pc);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::ReleaseGraphicsResources(vtkWindow* w)
{
  this->FrustumActor->ReleaseGraphicsResources(w);
  this->FarPlaneHorizontalHandle.Actor->ReleaseGraphicsResources(w);
  this->FarPlaneVerticalHandle.Actor->ReleaseGraphicsResources(w);
  this->NearPlaneEdgesHandle.Actor->ReleaseGraphicsResources(w);
  this->OriginHandle.Actor->ReleaseGraphicsResources(w);
  this->RollHandle.Actor->ReleaseGraphicsResources(w);
}

//------------------------------------------------------------------------------
int vtkImplicitFrustumRepresentation::RenderOpaqueGeometry(vtkViewport* v)
{
  int count = 0;
  this->BuildRepresentation();
  count += this->FarPlaneHorizontalHandle.Actor->RenderOpaqueGeometry(v);
  count += this->FarPlaneVerticalHandle.Actor->RenderOpaqueGeometry(v);
  count += this->NearPlaneEdgesHandle.Actor->RenderOpaqueGeometry(v);
  count += this->OriginHandle.Actor->RenderOpaqueGeometry(v);
  count += this->RollHandle.Actor->RenderOpaqueGeometry(v);

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
  count += this->OriginHandle.Actor->RenderTranslucentPolygonalGeometry(v);
  count += this->RollHandle.Actor->RenderTranslucentPolygonalGeometry(v);
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
  result |= this->OriginHandle.Actor->HasTranslucentPolygonalGeometry();
  result |= this->RollHandle.Actor->HasTranslucentPolygonalGeometry();

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
    case Scaling:
      os << "Scaling" << std::endl;
      break;
    case TranslatingOrigin:
      os << "TranslatingOrigin" << std::endl;
      break;
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
void vtkImplicitFrustumRepresentation::HighlightRollHandle(bool highlight)
{
  if (highlight)
  {
    this->RollHandle.Actor->SetProperty(this->SelectedEdgeHandleProperty);
  }
  else
  {
    this->RollHandle.Actor->SetProperty(this->EdgeHandleProperty);
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

  this->Origin = newOrigin;
  this->UpdateFrustumTransform();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::TranslateOriginOnAxis(
  const vtkVector3d& p1, const vtkVector3d& p2)
{
  vtkVector3d v = p2 - p1;

  // Add to the current point, project back down onto plane
  vtkVector3d axis(0, 1, 0);
  this->OrientationTransform->TransformVector(axis.GetData(), axis.GetData());
  axis.Normalize();

  // Project the point on the axis vector
  vtkVector3d newOrigin = this->Origin + v;
  vtkVector3d u = newOrigin - this->Origin;

  this->Origin = this->Origin + (axis * axis.Dot(u));
  ;
  this->UpdateFrustumTransform();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::Scale(
  const vtkVector3d& p1, const vtkVector3d& p2, double vtkNotUsed(X), double Y)
{
  // Get the motion vector
  vtkVector3d v = p2 - p1;

  // Compute the scale factor
  vtkBoundingBox bbox(this->WidgetBounds.GetData());
  double diagonal = bbox.GetDiagonalLength();
  if (diagonal == 0.)
  {
    return;
  }
  double sf = v.Norm() / diagonal;
  if (Y > this->LastEventPosition[1])
  {
    sf = 1.0 + sf;
  }
  else
  {
    sf = 1.0 - sf;
  }

  // TODO: This does not do anything !!!
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::UpdateFrustumTransform()
{
  vtkNew<vtkTransform> transform;
  transform->Identity();
  transform->Concatenate(this->OrientationTransform);
  transform->Translate(this->Origin.GetData());
  transform->Inverse();

  if (this->Frustum->GetTransform() != transform)
  {
    this->Frustum->SetTransform(transform);
    this->Modified();
  }
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
  this->SelectedEdgeHandleProperty->SetColor(r, g, b);
  this->SelectedOriginHandleProperty->SetColor(r, g, b);
  this->SelectedFrustumProperty->SetAmbientColor(r, g, b);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetHandleColor(double r, double g, double b)
{
  this->EdgeHandleProperty->SetColor(r, g, b);
  this->OriginHandleProperty->SetColor(r, g, b);
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

  for (int i = 0; i < 6; i++)
  {
    this->InitialBounds[i] = bounds[i];
    this->WidgetBounds[i] = bounds[i];
  }

  vtkBoundingBox bbox(bounds.GetData());
  this->InitialLength = bbox.GetDiagonalLength();
  this->Length = this->InitialLength;

  this->OrientationTransform->Identity();
  if (this->AlongXAxis)
  {
    this->OrientationTransform->RotateZ(90);
  }
  else if (this->AlongZAxis)
  {
    this->OrientationTransform->RotateX(90);
  }

  this->ValidPick = true; // since we have positioned the widget successfully
  this->BuildRepresentation();
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetOrigin(const vtkVector3d& xyz)
{
  if (xyz != this->Origin)
  {
    this->Origin = xyz;
    this->UpdateFrustumTransform();
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

//----------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetOrientation(const vtkVector3d& xyz)
{
  vtkVector3d orientation(this->OrientationTransform->GetOrientation());
  if (orientation != xyz)
  {
    // Orientation transform is in Post Multiply mode
    // so rotation order is YXZ
    this->OrientationTransform->Identity();
    this->OrientationTransform->RotateY(xyz.GetY());
    this->OrientationTransform->RotateX(xyz.GetX());
    this->OrientationTransform->RotateZ(xyz.GetZ());
    this->UpdateFrustumTransform();
  }
}

//----------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetOrientation(double x, double y, double z)
{
  this->SetOrientation(vtkVector3d(x, y, z));
}

//----------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::SetOrientation(const double xyz[3])
{
  this->SetOrientation(vtkVector3d(xyz));
}

//----------------------------------------------------------------------------
double* vtkImplicitFrustumRepresentation::GetOrientation()
{
  return this->OrientationTransform->GetOrientation();
}

//----------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetOrientation(double& x, double& y, double& z)
{
  vtkVector3d orientation(this->OrientationTransform->GetOrientation());
  x = orientation[0];
  y = orientation[1];
  z = orientation[2];
}

//----------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::GetOrientation(double xyz[3])
{
  vtkVector3d orientation(this->OrientationTransform->GetOrientation());
  xyz[0] = orientation[0];
  xyz[1] = orientation[1];
  xyz[2] = orientation[2];
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::Rotate(
  const vtkVector3d& prevPickPoint, const vtkVector3d& pickPoint, const vtkVector3d& axis)
{
  const vtkVector3d centeredP1 = prevPickPoint;
  const vtkVector3d centeredP2 = pickPoint;

  vtkVector3d rotationAxis(this->OrientationTransform->TransformVector(axis.GetData()));
  double rotationAngle = vtkMath::SignedAngleBetweenVectors(
    centeredP1.GetData(), centeredP2.GetData(), rotationAxis.GetData());

  this->OrientationTransform->RotateWXYZ(
    vtkMath::DegreesFromRadians(rotationAngle), rotationAxis.GetData());

  this->UpdateFrustumTransform();
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

  this->DrawFrustum = drawFrustum;
  this->Modified();
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
  if (this->Renderer == nullptr || !this->Renderer->GetRenderWindow())
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
    this->OriginHandle.Actor->SetPropertyKeys(info);

    vtkBoundingBox bbox(this->WidgetBounds.GetData());
    this->Length = std::max(bbox.GetMaxLength(), this->Frustum->GetNearPlaneDistance() * 1.1);

    vtkVector3d origin(this->GetOrigin());
    vtkVector3d forwardAxis(0, 1, 0);
    this->OrientationTransform->TransformVector(forwardAxis.GetData(), forwardAxis.GetData());

    // Set up the position handle
    vtkVector3d originHandlePosition = origin + forwardAxis * this->Frustum->GetNearPlaneDistance();
    this->OriginHandle.Source->SetCenter(originHandlePosition.GetData());

    // Place the roll control
    double minAngle =
      std::min(this->Frustum->GetHorizontalAngle(), this->Frustum->GetVerticalAngle());
    double rollHandleRadius = 0.9 * std::sin(vtkMath::RadiansFromDegrees(minAngle));

    // Roll handle is positionned right below the near plane
    vtkVector3d rollHandleCenter =
      origin + forwardAxis * (this->Frustum->GetNearPlaneDistance() - 0.1 * this->Length);
    this->RollHandle.Source->SetMajorRadiusVector(rollHandleRadius, 0, 0);
    this->RollHandle.Source->SetCenter(rollHandleCenter.GetData());
    this->RollHandle.Source->SetNormal(forwardAxis.GetData());

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

  this->OriginHandle.Source->SetRadius(radius);

  this->FarPlaneHorizontalHandle.Tuber->SetRadius(0.25 * radius);
  this->FarPlaneVerticalHandle.Tuber->SetRadius(0.25 * radius);
  this->NearPlaneEdgesHandle.Tuber->SetRadius(0.25 * radius);
  this->RollHandle.Tuber->SetRadius(0.25 * radius);
}

//------------------------------------------------------------------------------
void vtkImplicitFrustumRepresentation::BuildFrustum()
{
  const double height = this->Length;

  this->FrustumPD->Reset();

  // The edge between two faces of the frustum is given by the
  // the cross product between their normals
  vtkVector3d rightNormal(this->Frustum->GetRightPlane()->GetNormal());
  vtkVector3d leftNormal(this->Frustum->GetLeftPlane()->GetNormal());
  vtkVector3d bottomNormal(this->Frustum->GetBottomPlane()->GetNormal());
  vtkVector3d topNormal(this->Frustum->GetTopPlane()->GetNormal());

  std::array<vtkVector3d, 4> edgeDirections;
  edgeDirections[0] = bottomNormal.Cross(leftNormal).Normalized();  // bottom-left
  edgeDirections[1] = rightNormal.Cross(bottomNormal).Normalized(); // bottom-right
  edgeDirections[2] = topNormal.Cross(rightNormal).Normalized();    // top-right
  edgeDirections[3] = leftNormal.Cross(topNormal).Normalized();     // top-left

  vtkNew<vtkIdList> nearPlanePointIndices;
  nearPlanePointIndices->Allocate(4);
  vtkNew<vtkIdList> farPlanePointIndices;
  farPlanePointIndices->Allocate(4);

  vtkNew<vtkTransform> transform;
  transform->Identity();
  transform->Translate(this->Origin.GetData());
  transform->Concatenate(this->OrientationTransform);

  // Generate frustum points
  vtkPoints* frustumPoints = this->FrustumPD->GetPoints();
  for (const vtkVector3d& direction : edgeDirections)
  {
    // Scale the vector so that y in on the far or near plane
    vtkVector3d nearPoint = direction * (this->Frustum->GetNearPlaneDistance() / direction.GetY());
    vtkVector3d farPoint = direction * (height / direction.GetY());

    // Apply frustum transform
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
  farPlaneHorizontalLines->InsertNextCell({ 1, 2 });
  farPlaneHorizontalLines->InsertNextCell({ 3, 0 });

  this->FarPlaneHorizontalHandle.PolyData->Modified();

  auto farPlaneVerticalPoints = this->FarPlaneVerticalHandle.PolyData->GetPoints();
  frustumPoints->GetPoints(farPlanePointIndices, farPlaneVerticalPoints);

  auto farPlaneVerticalLines = this->FarPlaneVerticalHandle.PolyData->GetLines();
  farPlaneVerticalLines->InsertNextCell({ 0, 1 });
  farPlaneVerticalLines->InsertNextCell({ 2, 3 });

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

  frustum->SetTransform(this->Frustum->GetTransform());
  frustum->SetHorizontalAngle(this->Frustum->GetHorizontalAngle());
  frustum->SetVerticalAngle(this->Frustum->GetVerticalAngle());
  frustum->SetNearPlaneDistance(this->Frustum->GetNearPlaneDistance());
}

VTK_ABI_NAMESPACE_END
