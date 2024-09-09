// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkFrustum.h"
#include "vtkCylinder.h"
#include "vtkImplicitBoolean.h"
#include "vtkImplicitFunctionCollection.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkTransform.h"
#include "vtkVector.h"

#include <cmath>
#include <limits>

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkFrustum);

//------------------------------------------------------------------------------
vtkFrustum::vtkFrustum()
{
  this->NearPlane->SetNormal(0, 1, 0);
  this->NearPlane->SetOrigin(0, this->NearPlaneDistance, 0);

  this->CalculateHorizontalPlanesNormal();
  this->CalculateVerticalPlanesNormal();

  this->BooleanOp->AddFunction(this->NearPlane);
  this->BooleanOp->AddFunction(this->BottomPlane);
  this->BooleanOp->AddFunction(this->TopPlane);
  this->BooleanOp->AddFunction(this->RightPlane);
  this->BooleanOp->AddFunction(this->LeftPlane);

  this->BooleanOp->SetOperationTypeToUnion();
}

//------------------------------------------------------------------------------
vtkFrustum::~vtkFrustum() = default;

//------------------------------------------------------------------------------
double vtkFrustum::EvaluateFunction(double x[3])
{
  return this->BooleanOp->EvaluateFunction(x);
}

//------------------------------------------------------------------------------
void vtkFrustum::EvaluateGradient(double x[3], double g[3])
{
  return this->BooleanOp->EvaluateGradient(x, g);
}

//------------------------------------------------------------------------------
void vtkFrustum::SetOrigin(double x, double y, double z)
{
  this->SetOrigin(vtkVector3d(x, y, z));
}

//------------------------------------------------------------------------------
void vtkFrustum::SetOrigin(const double xyz[3])
{
  this->SetOrigin(vtkVector3d(xyz));
}

//------------------------------------------------------------------------------
void vtkFrustum::SetOrigin(const vtkVector3d& xyz)
{
  if (this->Origin != xyz)
  {
    this->Origin = xyz;
    this->UpdateTransform();
  }
}

//------------------------------------------------------------------------------
void vtkFrustum::GetOrigin(double& x, double& y, double& z)
{
  x = this->Origin[0];
  y = this->Origin[1];
  z = this->Origin[2];
}

//------------------------------------------------------------------------------
void vtkFrustum::GetOrigin(double xyz[3])
{
  this->GetOrigin(xyz[0], xyz[1], xyz[2]);
}

//------------------------------------------------------------------------------
double* vtkFrustum::GetOrigin()
{
  return this->Origin.GetData();
}

//------------------------------------------------------------------------------
void vtkFrustum::SetAxis(double x, double y, double z)
{
  this->SetAxis(vtkVector3d(x, y, z));
}

//------------------------------------------------------------------------------
void vtkFrustum::SetAxis(double axis[3])
{
  this->SetAxis(vtkVector3d(axis));
}

//------------------------------------------------------------------------------
void vtkFrustum::SetAxis(const vtkVector3d& axis)
{
  vtkVector3d newAxis = axis;

  // Normalize axis, reject if length == 0
  if (newAxis.Normalize() < std::numeric_limits<double>::epsilon())
  {
    return;
  }

  if (this->Axis != newAxis)
  {
    this->Axis = newAxis;
    this->UpdateTransform();
  }
}

//------------------------------------------------------------------------------
void vtkFrustum::GetAxis(double& x, double& y, double& z)
{
  x = this->Axis[0];
  y = this->Axis[1];
  z = this->Axis[2];
}

//------------------------------------------------------------------------------
void vtkFrustum::GetAxis(double xyz[3])
{
  xyz[0] = this->Axis[0];
  xyz[1] = this->Axis[1];
  xyz[2] = this->Axis[2];
}

//------------------------------------------------------------------------------
double* vtkFrustum::GetAxis()
{
  return this->Axis.GetData();
}

//------------------------------------------------------------------------------
void vtkFrustum::SetHorizontalAngle(double angleInDegrees)
{
  if (this->HorizontalAngle == angleInDegrees)
  {
    return;
  }

  this->HorizontalAngle = angleInDegrees;
  this->CalculateHorizontalPlanesNormal();
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkFrustum::SetVerticalAngle(double angleInDegrees)
{
  if (this->VerticalAngle == angleInDegrees)
  {
    return;
  }

  this->VerticalAngle = angleInDegrees;
  this->CalculateVerticalPlanesNormal();
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkFrustum::SetNearPlaneDistance(double distance)
{
  distance = std::min(distance, 0.0);
  if (this->NearPlaneDistance == distance)
  {
    return;
  }

  this->NearPlaneDistance = distance;
  this->NearPlane->SetOrigin(0, distance, 0);
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkFrustum::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Origin: " << this->Origin << std::endl;
  os << indent << "Axis: " << this->Axis << std::endl;
  // os << indent << "Inner Radius: " << this->InnerCylinder->GetRadius() << std::endl;
  // os << indent << "Outer Radius: " << this->OuterCylinder->GetRadius() << std::endl;
}

//------------------------------------------------------------------------------
void vtkFrustum::CalculateHorizontalPlanesNormal()
{
  // Maths!
  double angleRadians = vtkMath::RadiansFromDegrees(this->HorizontalAngle);
  auto cosAngle = std::cos(angleRadians);
  auto sinAngle = std::sin(angleRadians);

  vtkVector3d leftPlaneNormal(cosAngle, sinAngle, 0);
  vtkVector3d rightPlaneNormal(-cosAngle, sinAngle, 0);

  this->RightPlane->SetNormal(rightPlaneNormal.GetData());
  this->LeftPlane->SetNormal(leftPlaneNormal.GetData());
}

//------------------------------------------------------------------------------
void vtkFrustum::CalculateVerticalPlanesNormal()
{
  double angleRadians = vtkMath::RadiansFromDegrees(this->VerticalAngle);
  auto cosAngle = std::cos(angleRadians);
  auto sinAngle = std::sin(angleRadians);

  vtkVector3d topPlaneNormal(0, cosAngle, -sinAngle);
  vtkVector3d bottomPlaneNormal(0, cosAngle, sinAngle);

  this->TopPlane->SetNormal(topPlaneNormal.GetData());
  this->BottomPlane->SetNormal(bottomPlaneNormal.GetData());
}

//------------------------------------------------------------------------------
void vtkFrustum::UpdateTransform()
{
  const vtkVector3d yAxis(0., 1., 0.);

  vtkVector3d cross = yAxis.Cross(this->Axis);
  const double crossNorm = cross.Normalize();
  const double dot = yAxis.Dot(this->Axis);
  const double angle = vtkMath::DegreesFromRadians(std::atan2(crossNorm, dot));

  vtkNew<vtkTransform> transform;
  transform->Identity();
  transform->Translate(this->Origin.GetData());
  transform->RotateWXYZ(angle, cross.GetData());
  transform->Inverse();

  this->SetTransform(transform);
  this->Modified();
}

VTK_ABI_NAMESPACE_END
