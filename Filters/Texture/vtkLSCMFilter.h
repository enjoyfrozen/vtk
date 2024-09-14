// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class   vtkLSCMFilter
 * @brief   Compute least-squares comformal maps using chart ids on triangles.
 *
 * This filter takes input as a triangular surface mesh with chart assignment
 * for each triangle and outputs: (1) a vtkPartitionedDataSet of vtkPolyData
 * objects holding triangles of each chart with u-v coordinates and input
 * global id assigned to each point, and (2) a vtkPartitionedDataSet of
 * vtkPolyData objects holding polyline cells (VTK_POLY_LINE) corresponding to
 * seams of the chart and boundaries of the polydata. Each polydata have two
 * vtkIdTypeArray as cell data that mark each polyline with partition ID and
 * cell ID of its partner on another output polydata. Polylines representing
 * input boundaries have no partner and will have -1 for both cell data.
 *
 * The computation of LSCM is a least square problem with linear constraints
 * imposed by two anchors (points that are farthest in the input polydata
 * in the same chart). The problem is formulated into KKT equations and is
 * solved using QR decomposition with column pivoting. Within each chart, two
 * points that are farthest from each other in the input polydata are used as
 * the anchor points with anchorPoint0 assigned to origin and anchorPoint1
 * assigned to (0, originalDistance)
 *
 * The current computation of LSCM is implemented in sequential loops with
 * dense matrix operations using Eigen library.
 */

#ifndef vtkLSCMFilter_h
#define vtkLSCMFilter_h

#include "vtkFiltersTextureModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
#include "vtkSmartPointer.h" // For vtkSmartPointer

#include <vector> // For vector

VTK_ABI_NAMESPACE_BEGIN
class vtkPartitionedDataSet;

class VTKFILTERSTEXTURE_EXPORT vtkLSCMFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkLSCMFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkLSCMFilter(const vtkLSCMFilter&) = delete;
  vtkLSCMFilter& operator=(const vtkLSCMFilter&) = delete;

  /**
   * Construct object with
   */
  static vtkLSCMFilter* New();

  /// Used to index input-data arrays
  enum InputArrays
  {
    CHART_ID = 0 //!< The index of the input array holding chart IDs (must be cell-data).
  };

  /// Used to index output datasets
  enum OutputPorts
  {
    ATLAS = 0,    //!< The output texture atlas.
    BOUNDARY = 1, //!< Shared output chart boundaries.
    SEGMENTS = 2  //!< Original individual segments based on chart ids.
  };

  /// All outputs are partitioned data; provide methods that return this type:
  vtkPartitionedDataSet* GetOutput(int port = 0);
  vtkPartitionedDataSet* GetAtlasOutput() { return this->GetOutput(ATLAS); }
  vtkPartitionedDataSet* GetBoundaryOutput() { return this->GetOutput(BOUNDARY); }
  vtkPartitionedDataSet* GetSegmentsOutput() { return this->GetOutput(SEGMENTS); }

protected:
  vtkLSCMFilter();
  ~vtkLSCMFilter() override = default;

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int FillOutputPortInformation(int port, vtkInformation* info) override;
};
VTK_ABI_NAMESPACE_END
#endif
