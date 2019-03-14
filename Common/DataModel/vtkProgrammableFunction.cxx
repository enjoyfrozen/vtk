/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProgrammableFunction.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkProgrammableFunction.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkProgrammableFunction);

//----------------------------------------------------------------------------
// Construct sphere with center at (0,0,0) and radius=0.5.
vtkProgrammableFunction::vtkProgrammableFunction()
{
  this->F = nullptr;
  this->G = nullptr;
}

//----------------------------------------------------------------------------
// Specify user-defined function.
void vtkProgrammableFunction::
SetFunction(double (*f)(double x[3]))
{
  this->F = f;
}


//----------------------------------------------------------------------------
// Specify user-defined gradient function.
void vtkProgrammableFunction::
SetGradientFunction(void (*g)(double x[3], double n[3]))
{
  this->G = g;
}


//----------------------------------------------------------------------------
// Evaluate the function using the vtkImplicitFunction API.
double vtkProgrammableFunction::EvaluateFunction(double x[3])
{
  return (this->F == nullptr ? 0.0 : this->F(x));
}

//----------------------------------------------------------------------------
// Evaluate the gradient  using the vtkImplicitFunction API.
void vtkProgrammableFunction::EvaluateGradient(double x[3], double n[3])
{
  if ( this->G != nullptr )
  {
    this->G(x,n);
  }
  else
  {
    n[0] = n[1] = n[2] = 0.0;
  }
}

//----------------------------------------------------------------------------
void vtkProgrammableFunction::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Function: ("
     << (this->F == nullptr ? "not defined" : "defined") << ")\n";
  os << indent << "Gradient Function: ("
     << (this->G == nullptr ? "not defined" : "defined") << ")\n";
}
