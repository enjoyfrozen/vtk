// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class   vtkTextureAtlasGenerator
 *
 * vtkTextureAtlasGenerator is a filter to generate 2-dimensional texture coordinates on
 * input polydata surfaces, implementing the paper "Least Squares Conformal Maps for
 * Automatic Texture Atlas Generation", Bruno Levy et al. 2002.
 *
 * It does this by (1) identifying feature edges from input surface, (2) computing distances
 * along the surface from these edges, (3) locating points as far from the edges as possible,
 * (4) growing "charts" of the surface out from these points until they partition the input
 * surface while minimizing geometry distortion as the chart is projected to a plane,
 * (5) packing the charts into a rectangular region of the texture-coordinate plane without
 * any overlap, and (6) reconstituting the input surface with the given texture coordinates.
 *
 * The output surface will generally turn input feature edges into seams where charts are glued
 * together. These seams common to a pair of charts are identified in the output and can be used
 * to force texture continuity across seams in the parameterization.
 */

#ifndef vtkTextureAtlasGenerator_h
#define vtkTextureAtlasGenerator_h

#include "vtkFiltersParallelDIY2Module.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

VTK_ABI_NAMESPACE_BEGIN
class VTKFILTERSPARALLELDIY2_EXPORT vtkTextureAtlasGenerator : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkTextureAtlasGenerator, vtkPolyDataAlgorithm);
  void PrintSelf(ostream&, vtkIndent) override {}

  /**
   * Construct an instance with feature angle = 30; all types of edges
   * (except manifold edges) are extracted and colored.
   */
  static vtkTextureAtlasGenerator* New();

  /**
   * Returns the name of the output texture coordinate array.
   */
  static const char* UVCoordinatesArrayName() { return "uv_atlas"; }

  vtkTextureAtlasGenerator();

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

private:
  vtkTextureAtlasGenerator(const vtkTextureAtlasGenerator&) = delete;
  vtkTextureAtlasGenerator& operator=(const vtkTextureAtlasGenerator&) = delete;
};
VTK_ABI_NAMESPACE_END
#endif
