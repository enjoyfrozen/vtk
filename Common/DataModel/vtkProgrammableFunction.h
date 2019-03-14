/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProgrammableFunction.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkProgrammableFunction
 * @brief   define a user-specified implicit function
 *
 * vtkProgrammableFunction evaluates a user-defined function at a
 * point in space and returns a value and/or three-component
 * gradient. vtkProgrammableFunction is a concrete implementation of
 * vtkImplicitFunction.
 *
 * This class is typically used to define complex functions that need
 * to interface with the VTK implicit function framework. Implicit
 * functions can be used for modelling, selecting, and extracting
 * geometry from datasets.
 */

#ifndef vtkProgrammableFunction_h
#define vtkProgrammableFunction_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkImplicitFunction.h"

class VTKCOMMONDATAMODEL_EXPORT vtkProgrammableFunction : public vtkImplicitFunction
{
public:
  //@{
  /**
   * Standard methods for instantiation, type information, and printing.
   */
  static vtkProgrammableFunction *New();
  vtkTypeMacro(vtkProgrammableFunction,vtkImplicitFunction);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  //@{
  /**
   * Specify the user-defined function which takes a point x[3] and
   * returns a value.
   */
  void SetFunction(double (*f)(double x[3]));
  //@}

  //@{
  /**
   * Specify the user-defined function which takes a point x[3] and
   * returns a gradient vector n[3].
   */
  void SetGradientFunction(void (*g)(double x[3], double n[3]));
  //@}

  //@{
  /**
   * Evaluate the user-defined function at the point x[3].
   */
  using vtkImplicitFunction::EvaluateFunction;
  double EvaluateFunction(double x[3]) override;
  //@}

  /**
   * Evaluate the gradient n[3] of the user-defined function at the
   * point x[3].
   */
  void EvaluateGradient(double x[3], double n[3]) override;

protected:
  vtkProgrammableFunction();
  ~vtkProgrammableFunction() override {}

  double (*F)(double x[3]);
  void (*G)(double x[3], double n[3]);

private:
  vtkProgrammableFunction(const vtkProgrammableFunction&) = delete;
  void operator=(const vtkProgrammableFunction&) = delete;
};

#endif
