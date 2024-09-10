// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class   vtkFrustum
 * @brief   implicit function for a frustum
 *
 * vtkFrustum represents a 4-sided frustum, with a near plane but infinite on the far side. Opposite
 * sides of its base are always the same length. vtkFrustum is a concrete implementation of
 * vtkImplicitFunction. You can redefine the origin and axis of rotation by setting the Origin and
 * Axis data members. (Note that it is also possible to use the superclass' vtkImplicitFunction
 * transformation matrix if necessary to reposition by using FunctionValue() and
 * FunctionGradient().)
 *
 * @warning
 * The frustum is infinite in extent. To truncate the frustum in
 * modeling operations use the vtkImplicitBoolean in combination with
 * clipping planes.
 *
 */

#ifndef vtkFrustum_h
#define vtkFrustum_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkImplicitFunction.h"
#include "vtkNew.h" // For vtkNew

VTK_ABI_NAMESPACE_BEGIN
class vtkPlane;
class vtkImplicitBoolean;

class VTKCOMMONDATAMODEL_EXPORT vtkFrustum : public vtkImplicitFunction
{
public:
  static vtkFrustum* New();
  vtkTypeMacro(vtkFrustum, vtkImplicitFunction);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  using vtkImplicitFunction::EvaluateFunction;
  double EvaluateFunction(double x[3]) override;

  void EvaluateGradient(double x[3], double g[3]) override;

  vtkGetMacro(NearPlaneDistance, double)
  void SetNearPlaneDistance(double distance);

  vtkGetMacro(VerticalAngle, double)
  void SetVerticalAngle(double angleInDegrees);

  vtkGetMacro(HorizontalAngle, double)
  void SetHorizontalAngle(double angleInDegrees);

protected:
  vtkFrustum();
  ~vtkFrustum() override;

private:
  vtkFrustum(const vtkFrustum&) = delete;
  void operator=(const vtkFrustum&) = delete;

  void CalculateHorizontalPlanesNormal();
  void CalculateVerticalPlanesNormal();

  double NearPlaneDistance = 0.5;
  double VerticalAngle = 30;
  double HorizontalAngle = 30;

  vtkNew<vtkPlane> NearPlane;
  vtkNew<vtkPlane> BottomPlane;
  vtkNew<vtkPlane> TopPlane;
  vtkNew<vtkPlane> RightPlane;
  vtkNew<vtkPlane> LeftPlane;

  vtkNew<vtkImplicitBoolean> BooleanOp;
};

VTK_ABI_NAMESPACE_END
#endif
