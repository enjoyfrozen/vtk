/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLagrangianParticleTracker.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

    This software is distributed WITHOUT ANY WARRANTY; without even
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// created by Shaun David Ramsey and Kristin Potter copyright (c) 2003
// email ramsey()cs.utah.edu with any questions
/*=========================================================================
  This copyright notice is available at:
http://www.opensource.org/licenses/mit-license.php

Copyright (c) 2003 Shaun David Ramsey, Kristin Potter, Charles Hansen

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sel copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
=========================================================================*/
/**
 * @class   vtkBilinearQuadIntersection
 * @brief   Class to perform non planar quad intersection
 *
 * Class for non planar intersection.
 * This class is based on http://shaunramsey.com/research/bp/
 * which does not work in the general case hence the ugly transformation patch.
 */

#ifndef vtkBilinearQuadIntersection_h
#define vtkBilinearQuadIntersection_h

#include "vtkFiltersFlowPathsModule.h" // For export macro
#include "vtkVector.h"

class VTKFILTERSFLOWPATHS_EXPORT vtkBilinearQuadIntersection
{
public:
  vtkBilinearQuadIntersection(const vtkVector3d& pt00, const vtkVector3d& Pt01,
    const vtkVector3d& Pt10, const vtkVector3d& Pt11);
  vtkBilinearQuadIntersection() = default;
  ~vtkBilinearQuadIntersection() = default;

  //@{
  /**
   * Get direct access to the underlying point data
   */
  double* GetP00Data();
  double* GetP01Data();
  double* GetP10Data();
  double* GetP11Data();
  //}@

  /**
   * Compute cartesian coordinates of point in the quad
   * using parameteric coordinates
   */
  vtkVector3d ComputeCartesianCoordinates(double u, double v);

  /**
   * Compute the intersection between a ray r->d and the quad
   */
  bool RayIntersection(const vtkVector3d& r, const vtkVector3d& d, vtkVector3d& uv);

  /**
   * find roots of ax^2+bx+c=0  in the interval min,max.
   * place the roots in u[2] and return how many roots found
   */
  static int QuadraticRoot(double a, double b, double c, double min, double max, double* u);

  /**
   * Compute intersection factor
   */
  static double ComputeIntersectionFactor(
    const vtkVector3d& dir, const vtkVector3d& orig, const vtkVector3d& srfpos);

  /**
   * Compute best denominator
   */
  static double GetBestDenominator(double v, double m1, double m2, double J1, double J2, double K1,
    double K2, double R1, double R2);

private:
  vtkVector3d Point00;
  vtkVector3d Point01;
  vtkVector3d Point10;
  vtkVector3d Point11;
  int AxesSwapping = 0;
};
#endif // vtkBilinearQuadIntersection_h
