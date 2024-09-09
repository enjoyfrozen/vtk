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
#include "vtkPlane.h"
#include "vtkSetGet.h"
#include "vtkVector.h" // For vtkVector3d

VTK_ABI_NAMESPACE_BEGIN
class vtkCylinder;
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

  ///@{
  /**
   * Set/Get the frustum origin. Default is (0, 0, 0).
   */
  void SetOrigin(double x, double y, double z);
  void SetOrigin(const double xyz[3]);
  void SetOrigin(const vtkVector3d& xyz);
  void GetOrigin(double& x, double& y, double& z);
  void GetOrigin(double xyz[3]);
  double* GetOrigin() VTK_SIZEHINT(3);
  ///@}

  ///@{
  /**
   * Set/Get the forward axis of the frustum. If the axis is not specified as
   * a unit vector, it will be normalized. If zero-length axis vector
   * is used as input to this method, it will be ignored.
   * Default is the Y-axis (0, 1, 0)
   */
  void SetAxis(double x, double y, double z);
  void SetAxis(double axis[3]);
  void SetAxis(const vtkVector3d& axis);
  void GetAxis(double& x, double& y, double& z);
  void GetAxis(double xyz[3]);
  double* GetAxis() VTK_SIZEHINT(3);
  ///@}

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

  void UpdateTransform();
  void CalculateHorizontalPlanesNormal();
  void CalculateVerticalPlanesNormal();

  vtkVector3d Origin = { 0.0, 0.0, 0.0 };
  vtkVector3d Axis = { 0.0, 1.0, 0.0 };
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
