/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkGraphWeightEuclideanDistanceFilter
 * @brief   Weights the edges of a
 * graph based on the Euclidean distance between the points.
 *
 *
 * Weights the edges of a graph based on the Euclidean distance between the points.
*/

#ifndef vtkGraphWeightEuclideanDistanceFilter_h
#define vtkGraphWeightEuclideanDistanceFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkGraphWeightFilter.h"

class vtkGraph;

class VTKFILTERSGENERAL_EXPORT vtkGraphWeightEuclideanDistanceFilter : public vtkGraphWeightFilter
{
 public:
  static vtkGraphWeightEuclideanDistanceFilter *New();
  vtkTypeMacro(vtkGraphWeightEuclideanDistanceFilter, vtkGraphWeightFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkGraphWeightEuclideanDistanceFilter(){}
  ~vtkGraphWeightEuclideanDistanceFilter() override {}

  /**
   * Compute the Euclidean distance between the Points defined for the
   * vertices of a specified 'edge'.
   */
  float ComputeWeight(vtkGraph* const graph, const vtkEdgeType& edge) const override;

  /**
   * Ensure that 'graph' has Points defined.
   */
  bool CheckRequirements(vtkGraph* const graph) const override;

private:
  vtkGraphWeightEuclideanDistanceFilter(const vtkGraphWeightEuclideanDistanceFilter&) = delete;
  void operator=(const vtkGraphWeightEuclideanDistanceFilter&) = delete;
};

#endif
