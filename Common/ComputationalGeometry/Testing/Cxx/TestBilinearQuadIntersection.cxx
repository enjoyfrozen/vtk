/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestLagrangianParticle.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkBilinearQuadIntersection.h"

#include "vtkMathUtilities.h"

#include <iostream>

int TestBilinearQuadIntersection(int, char*[])
{
  // --------- Static methods ------------
  // ---- GetBestDenominator ----
  double denom =
    vtkBilinearQuadIntersection::GetBestDenominator(1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9);
  if (!vtkMathUtilities::FuzzyCompare(denom, -1.30882352941, 1e-6))
  {
    std::cerr << setprecision(12);
    std::cerr << "vtkBilinearQuadIntersection::GetBestDenominator expecting -1.30882352941, got"
              << denom << std::endl;
    return EXIT_FAILURE;
  }

  // ---- QuadraticRoot ----
  double root[2];
  int numRoot;

  // x2 = 0
  numRoot = vtkBilinearQuadIntersection::QuadraticRoot(1, 0, 0, -10, 10, root);
  if (numRoot != 1 || root[0] != 0)
  {
    std::cerr << "vtkBilinearQuadIntersection::QuadraticRoot got unexpected results" << std::endl;
    std::cerr << "numRoot should be 1, it is " << numRoot << std::endl;
    std::cerr << "root should be 0, it is " << root[0] << std::endl;
    return EXIT_FAILURE;
  }
  // x2 - x - 2 = 0
  numRoot = vtkBilinearQuadIntersection::QuadraticRoot(1, -1, -2, -10, 10, root);
  if (numRoot != 2 || root[0] != -1 || root[1] != 2)
  {
    std::cerr << "vtkBilinearQuadIntersection::QuadraticRoot got unexpected results" << std::endl;
    std::cerr << "numRoot should be 2, it is " << numRoot << std::endl;
    std::cerr << "root should be -1, it is " << root[0] << std::endl;
    std::cerr << "root should be 2, it is " << root[1] << std::endl;
    return EXIT_FAILURE;
  }

  // ---- ComputeIntersectionFactor ----
  vtkVector3d dir(1, 2, 1);
  vtkVector3d orig(0, 0, 0);
  vtkVector3d srfpos(0.5, 0.5, 0.5);
  double factor = vtkBilinearQuadIntersection::ComputeIntersectionFactor(dir, orig, srfpos);
  if (factor != 0.25)
  {
    std::cerr << "vtkBilinearQuadIntersection::ComputeIntersectionFactor expecting 0.5, got"
              << factor << std::endl;
    return EXIT_FAILURE;
  }

  // -------- Member methods --------
  vtkVector3d p00(0, 0, 0);
  vtkVector3d p01(0, 1, 0);
  vtkVector3d p10(1, 0, 0);
  vtkVector3d p11(1, 1, 0.5);
  vtkBilinearQuadIntersection quad(p00, p01, p10, p11);

  // ---- ComputeCartesianCoordinates ----
  double u = 0.3;
  double v = 0.7;
  vtkVector3d coord = quad.ComputeCartesianCoordinates(u, v);
  if (coord.GetX() != u || coord.GetY() != v || coord.GetZ() != 0.105)
  {
    std::cerr << "vtkBilinearQuadIntersection::ComputeCartesianCoordinates got unexpected results :"
              << std::endl;
    std::cerr << coord.GetX() << " " << coord.GetY() << " " << coord.GetZ() << std::endl;
    return EXIT_FAILURE;
  }

  // ---- RayIntersection ----
  vtkVector3d r(0.5, 0.5, -1);
  vtkVector3d q(0, 0, 1);
  vtkVector3d uv;
  quad.RayIntersection(r, q, uv);
  if (uv.GetX() != 0.5 || uv.GetY() != 0.5 || uv.GetZ() != 1.125)
  {
    std::cerr << "vtkBilinearQuadIntersection::RayIntersection got unexpected results :"
              << std::endl;
    std::cerr << (uv.GetX() != 0.5) << " " << (uv.GetY() != 0.5) << " " << (uv.GetZ() != 1.125)
              << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
