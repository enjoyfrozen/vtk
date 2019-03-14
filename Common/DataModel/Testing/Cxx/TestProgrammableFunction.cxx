/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestProgrammableFunction.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME
// .SECTION Description
// this program tests the programmable function class.

#include "vtkProgrammableFunction.h"
#include "vtkSmartPointer.h"

double SphereFunction(double x[3])
{
  return (x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
}

void SphereGradient(double x[3], double n[3])
{
  n[0] = 2.0 * x[0];
  n[1] = 2.0 * x[1];
  n[2] = 2.0 * x[2];
}

int TestProgrammableFunction(int,char *[])
{
  auto pf = vtkSmartPointer<vtkProgrammableFunction>::New();

  pf->SetFunction(SphereFunction);
  pf->SetGradientFunction(SphereGradient);


  double val, x[3]={0.0,0.0,10.0};
  val = pf->EvaluateFunction(x);
  if ( val != 100.0 )
  {
    cerr << "ERROR:  incorrect function value" << endl;
    return EXIT_FAILURE;
  }


  double n[3];
  pf->EvaluateGradient(x,n);
  if ( n[0] != 0.0 || n[1] != 0.0 || n[2] != 20.0 )
  {
    cerr << "ERROR:  incorrect gradient" << endl;
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}
