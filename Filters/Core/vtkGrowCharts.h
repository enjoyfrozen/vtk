// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#ifndef vtkGrowCharts_h
#define vtkGrowCharts_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

/**
 * @class   vtkGrowCharts
 * @brief   Compute the distance of every point to the nearest feature.
 *
 * This filter takes as input a mesh that has an input cell data array
 * representing a distance to feature edges over the mesh. It has to be set using
 * `vtkAlgorithm::SetInputArrayToProcess`
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

VTK_ABI_NAMESPACE_BEGIN
class VTKFILTERSCORE_EXPORT vtkGrowCharts : public vtkPolyDataAlgorithm
{
public:
  static vtkGrowCharts* New();
  vtkTypeMacro(vtkGrowCharts, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Name of the output chart id array.
   */
  static const char* ChartIdArrayName() { return "chart_id"; }

protected:
  vtkGrowCharts();
  ~vtkGrowCharts() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

private:
  vtkGrowCharts(const vtkGrowCharts&) = delete;
  void operator=(const vtkGrowCharts&) = delete;
};
VTK_ABI_NAMESPACE_END

#endif
