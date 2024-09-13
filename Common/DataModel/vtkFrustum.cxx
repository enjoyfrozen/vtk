// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkFrustum.h"
#include "vtkImplicitBoolean.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkVector.h"

#include <cmath>

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
void vtkFrustum::SetHorizontalAngle(double angleInDegrees)
{
  angleInDegrees = vtkMath::ClampValue(angleInDegrees, 1., 89.);
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
  angleInDegrees = vtkMath::ClampValue(angleInDegrees, 1., 89.);
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
  distance = std::max(distance, 0.0);
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
  // os << indent << "Origin: " << this->Origin << std::endl;
  // os << indent << "Axis: " << this->Axis << std::endl;
  // os << indent << "Inner Radius: " << this->InnerCylinder->GetRadius() << std::endl;
  // os << indent << "Outer Radius: " << this->OuterCylinder->GetRadius() << std::endl;
}

//------------------------------------------------------------------------------
void vtkFrustum::CalculateHorizontalPlanesNormal()
{
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

  vtkVector3d topPlaneNormal(0, sinAngle, -cosAngle);
  vtkVector3d bottomPlaneNormal(0, sinAngle, cosAngle);

  this->TopPlane->SetNormal(topPlaneNormal.GetData());
  this->BottomPlane->SetNormal(bottomPlaneNormal.GetData());
}

VTK_ABI_NAMESPACE_END
