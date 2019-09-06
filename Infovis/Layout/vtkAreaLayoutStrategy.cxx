/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkAreaLayoutStrategy.h"

#include "vtkTree.h"


vtkAreaLayoutStrategy::vtkAreaLayoutStrategy()
{
  this->ShrinkPercentage = 0.0;
}

vtkAreaLayoutStrategy::~vtkAreaLayoutStrategy() = default;

void vtkAreaLayoutStrategy::LayoutEdgePoints(
  vtkTree* inputTree,
  vtkDataArray* vtkNotUsed(coordsArray),
  vtkDataArray* vtkNotUsed(sizeArray),
  vtkTree* edgeRoutingTree)
{
  edgeRoutingTree->ShallowCopy(inputTree);
}

void vtkAreaLayoutStrategy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "ShrinkPercentage: " << this->ShrinkPercentage << endl;
}


