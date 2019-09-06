/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkPruneTreeFilter.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkTree.h"

//----------------------------------------------------------------------------
int TestPruneTreeFilter(int, char*[])
{
  vtkNew<vtkMutableDirectedGraph> graph;
  vtkIdType root = graph->AddVertex();
  vtkIdType internalOne = graph->AddChild(root);
  vtkIdType internalTwo = graph->AddChild(internalOne);
  vtkIdType a = graph->AddChild(internalTwo);
  graph->AddChild(internalTwo);
  graph->AddChild(internalOne);
  graph->AddChild(a);
  graph->AddChild(a);

  vtkNew<vtkTree> tree;
  tree->ShallowCopy(graph);

  vtkNew<vtkPruneTreeFilter> filter;
  filter->SetInputData(tree);
  filter->SetParentVertex(internalTwo);
  vtkTree *prunedTree = filter->GetOutput();
  filter->Update();

  if (prunedTree->GetNumberOfVertices() == 3)
  {
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
