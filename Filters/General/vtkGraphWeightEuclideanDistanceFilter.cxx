/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkGraphWeightEuclideanDistanceFilter.h"

#include "vtkGraph.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkGraphWeightEuclideanDistanceFilter);

float vtkGraphWeightEuclideanDistanceFilter::ComputeWeight(vtkGraph* const graph,
                                                           const vtkEdgeType& edge) const
{
  double p1[3];
  graph->GetPoint(edge.Source, p1);

  double p2[3];
  graph->GetPoint(edge.Target, p2);

  float w = static_cast<float>(sqrt(vtkMath::Distance2BetweenPoints(p1, p2)));

  return w;
}

bool vtkGraphWeightEuclideanDistanceFilter::CheckRequirements(vtkGraph* const graph) const
{
  vtkPoints* points = graph->GetPoints();
  if(!points)
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
void vtkGraphWeightEuclideanDistanceFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkGraphWeightFilter::PrintSelf(os,indent);
}
