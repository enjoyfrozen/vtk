// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkWarpTransform.h"
#include "vtkMath.h"

//------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_BEGIN
void vtkWarpTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "InverseFlag: " << this->InverseFlag << "\n";
  os << indent << "InverseTolerance: " << this->InverseTolerance << "\n";
  os << indent << "InverseIterations: " << this->InverseIterations << "\n";
}

//------------------------------------------------------------------------------
vtkWarpTransform::vtkWarpTransform()
{
  this->InverseFlag = 0;
  this->InverseTolerance = 0.001;
  this->InverseIterations = 500;
}

//------------------------------------------------------------------------------
vtkWarpTransform::~vtkWarpTransform() = default;

//------------------------------------------------------------------------------
// Check the InverseFlag, and perform a forward or reverse transform
// as appropriate.
void vtkWarpTransform::InternalTransformPoint(const double input[3], double output[3])
{
  if (this->InverseFlag)
  {
    this->InverseTransformPoint(input, output);
  }
  else
  {
    this->ForwardTransformPoint(input, output);
  }
}

void vtkWarpTransform::InternalTransformPoint(const float input[3], float output[3])
{
  if (this->InverseFlag)
  {
    this->InverseTransformPoint(input, output);
  }
  else
  {
    this->ForwardTransformPoint(input, output);
  }
}

//------------------------------------------------------------------------------
// Check the InverseFlag, and set the output point and derivative as
// appropriate.
void vtkWarpTransform::InternalTransformDerivative(
  const double input[3], double output[3], double derivative[3][3])
{
  if (this->InverseFlag)
  {
    this->InverseTransformDerivative(input, output, derivative);
    vtkMath::Invert3x3(derivative, derivative);
  }
  else
  {
    this->ForwardTransformDerivative(input, output, derivative);
  }
}

void vtkWarpTransform::InternalTransformDerivative(
  const float input[3], float output[3], float derivative[3][3])
{
  if (this->InverseFlag)
  {
    this->InverseTransformDerivative(input, output, derivative);
    vtkMath::Invert3x3(derivative, derivative);
  }
  else
  {
    this->ForwardTransformDerivative(input, output, derivative);
  }
}

//------------------------------------------------------------------------------
// We use Newton's method to iteratively invert the transformation.
// This is actually quite robust as long as the Jacobian matrix is never
// singular.
void vtkWarpTransform::InverseTransformDerivative(
  const double point[3], double output[3], double derivative[3][3])
{
  double inverse[3], lastInverse[3];
  double deltaP[3], deltaI[3];

  double functionDerivative = 0;
  double lastFunctionValue = VTK_DOUBLE_MAX;

  double errorSquared = 0;
  double toleranceSquared = this->GetInverseTolerance();
  toleranceSquared *= toleranceSquared;

  double f = 1.0;
  double a;

  // first guess at inverse point: invert the displacement
  this->TemplateTransformPoint(point, inverse);

  inverse[0] -= 2 * (inverse[0] - point[0]);
  inverse[1] -= 2 * (inverse[1] - point[1]);
  inverse[2] -= 2 * (inverse[2] - point[2]);

  lastInverse[0] = inverse[0];
  lastInverse[1] = inverse[1];
  lastInverse[2] = inverse[2];

  // do a maximum 500 iterations, usually less than 10 are required
  int n = this->GetInverseIterations();
  int i;

  for (i = 0; i < n; i++)
  {
    // put the inverse point back through the transform
    this->TemplateTransformPoint(inverse, deltaP, derivative);

    // how far off are we?
    deltaP[0] -= point[0];
    deltaP[1] -= point[1];
    deltaP[2] -= point[2];

    // get the current function value
    double functionValue = (deltaP[0] * deltaP[0] + deltaP[1] * deltaP[1] + deltaP[2] * deltaP[2]);

    // if the function value is decreasing, do next Newton step
    // (the check on f is to ensure that we don't do too many
    // reduction steps between the Newton steps)
    if (i == 0 || functionValue < lastFunctionValue || f < 0.05)
    {
      // here is the critical step in Newton's method
      vtkMath::LinearSolve3x3(derivative, deltaP, deltaI);

      // get the error value in the output coord space
      errorSquared = (deltaI[0] * deltaI[0] + deltaI[1] * deltaI[1] + deltaI[2] * deltaI[2]);

      // break if less than tolerance in both coordinate systems
      if (errorSquared < toleranceSquared && functionValue < toleranceSquared)
      {
        break;
      }

      // save the last inverse point
      lastInverse[0] = inverse[0];
      lastInverse[1] = inverse[1];
      lastInverse[2] = inverse[2];

      // save the function value at that point
      lastFunctionValue = functionValue;

      // derivative of functionValue at last inverse point
      functionDerivative =
        (deltaP[0] * derivative[0][0] * deltaI[0] + deltaP[1] * derivative[1][1] * deltaI[1] +
          deltaP[2] * derivative[2][2] * deltaI[2]) *
        2;

      // calculate new inverse point
      inverse[0] -= deltaI[0];
      inverse[1] -= deltaI[1];
      inverse[2] -= deltaI[2];

      // reset f to 1.0
      f = 1.0;

      continue;
    }

    // the error is increasing, so take a partial step
    // (see Numerical Recipes 9.7 for rationale, this code
    //  is a simplification of the algorithm provided there)

    // quadratic approximation to find best fractional distance
    a = -functionDerivative / (2 * (functionValue - lastFunctionValue - functionDerivative));

    // clamp to range [0.1,0.5]
    f *= (a < 0.1 ? 0.1 : (a > 0.5 ? 0.5 : a));

    // re-calculate inverse using fractional distance
    inverse[0] = lastInverse[0] - f * deltaI[0];
    inverse[1] = lastInverse[1] - f * deltaI[1];
    inverse[2] = lastInverse[2] - f * deltaI[2];
  }

  vtkDebugMacro("Inverse Iterations: " << (i + 1));

  if (i >= n)
  {
    // didn't converge: back up to last good result
    inverse[0] = lastInverse[0];
    inverse[1] = lastInverse[1];
    inverse[2] = lastInverse[2];

    if (this->IncrementErrorsSinceUpdate() == 1)
    {
      // print warning: Newton's method didn't converge
      vtkWarningMacro("InverseTransformPoint: no convergence ("
        << point[0] << ", " << point[1] << ", " << point[2] << ") error = " << sqrt(errorSquared)
        << " after " << i << " iterations.");
    }
  }

  output[0] = inverse[0];
  output[1] = inverse[1];
  output[2] = inverse[2];
}

void vtkWarpTransform::InverseTransformDerivative(
  const float point[3], float output[3], float derivative[3][3])
{
  double fpoint[3];
  double fderivative[3][3];
  fpoint[0] = point[0];
  fpoint[1] = point[1];
  fpoint[2] = point[2];

  this->InverseTransformDerivative(fpoint, fpoint, fderivative);

  for (int i = 0; i < 3; i++)
  {
    output[i] = static_cast<float>(fpoint[i]);
    derivative[i][0] = static_cast<float>(fderivative[i][0]);
    derivative[i][1] = static_cast<float>(fderivative[i][1]);
    derivative[i][2] = static_cast<float>(fderivative[i][2]);
  }
}

//------------------------------------------------------------------------------
void vtkWarpTransform::InverseTransformPoint(const double point[3], double output[3])
{
  // the derivative won't be used, but it is required for Newton's method
  double derivative[3][3];
  this->InverseTransformDerivative(point, output, derivative);
}

void vtkWarpTransform::InverseTransformPoint(const float point[3], float output[3])
{
  double fpoint[3];
  double fderivative[3][3];
  fpoint[0] = point[0];
  fpoint[1] = point[1];
  fpoint[2] = point[2];

  this->InverseTransformDerivative(fpoint, fpoint, fderivative);

  output[0] = static_cast<float>(fpoint[0]);
  output[1] = static_cast<float>(fpoint[1]);
  output[2] = static_cast<float>(fpoint[2]);
}

//------------------------------------------------------------------------------
// To invert the transformation, just set the InverseFlag.
void vtkWarpTransform::Inverse()
{
  this->InverseFlag = !this->InverseFlag;
  this->Modified();
}
VTK_ABI_NAMESPACE_END
