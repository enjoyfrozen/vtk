// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#ifndef vtkDistanceToFeature_h
#define vtkDistanceToFeature_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPassInputTypeAlgorithm.h"

/**
 * @class   vtkDistanceToFeature
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

VTK_ABI_NAMESPACE_BEGIN
class VTKFILTERSCORE_EXPORT vtkDistanceToFeature : public vtkPassInputTypeAlgorithm
{
public:
  static vtkDistanceToFeature* New();
  vtkTypeMacro(vtkDistanceToFeature, vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkDistanceToFeature(const vtkDistanceToFeature&) = delete;
  vtkDistanceToFeature& operator=(const vtkDistanceToFeature&) = delete;

  //@{
  /**
   * Specify the source object describing features.
   * This object is used to generate starting points (seeds).
   * Note that this method does not connect the pipeline. The algorithm will
   * work on the input data as it is without updating the producer of the data.
   * See SetSourceConnection for connecting the pipeline.
   */
  void SetSourceData(vtkPolyData* source);
  vtkPolyData* GetSource();
  //@}

  /**
   * Specify the source object describing features.
   * This object is used to generate starting points (seeds).
   * New style.
   */
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

  /**
   * Name of the outputted array.
   */
  static const char* OutputArrayName() { return "distance to feature"; }

protected:
  vtkDistanceToFeature();
  ~vtkDistanceToFeature() override;

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;
};
VTK_ABI_NAMESPACE_END

#endif
