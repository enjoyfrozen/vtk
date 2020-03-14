/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGrowCharts.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef vtkGrowCharts_h
#define vtkGrowCharts_h

#include "vtkFiltersGeometryModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

/**
 * @class   vtkGrowCharts
 * @brief   Compute the distance of every point to the nearest feature.
 *
 * This algorithm requires 2 inputs: a surface whose points have global
 * IDs assigned and a series of edges from the surface (also with the
 * same global IDs assigned). The result is 2 point-data arrays named
 * "distance to feature" and "nearest feature".
 * Distance is measured in arc length along edges of the input surface.
 *
 * A second output contains the input feature edges with an additional
 * cell-data array named "feature id".
 *
 * If the second input is not provided or has no edges, then
 * 2 points will be chosen as features. The points will be
 * those nearest the 2 largest faces of the first input's axis-aligned
 * bounding box. If the bounding box is a cube, points with the
 * smallest and largest z-axis coordinates will be used.
 *
 * This is one phase in least-squares conformal mapping (LSCM), a
 * technique used to generate a parameterization of a surface such
 * as a texture atlas.
 */
class VTKFILTERSGEOMETRY_EXPORT vtkGrowCharts : public vtkPolyDataAlgorithm
{
public:
  static vtkGrowCharts* New();
  vtkTypeMacro(vtkGrowCharts, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkGrowCharts();
  ~vtkGrowCharts() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int FillOutputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkGrowCharts(const vtkGrowCharts&) = delete;
  void operator=(const vtkGrowCharts&) = delete;
};

#endif
