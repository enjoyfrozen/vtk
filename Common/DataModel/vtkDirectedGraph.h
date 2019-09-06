/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkDirectedGraph
 * @brief   A directed graph.
 *
 *
 * vtkDirectedGraph is a collection of vertices along with a collection of
 * directed edges (edges that have a source and target). ShallowCopy()
 * and DeepCopy() (and CheckedShallowCopy(), CheckedDeepCopy())
 * accept instances of vtkTree and vtkMutableDirectedGraph.
 *
 * vtkDirectedGraph is read-only. To create an undirected graph,
 * use an instance of vtkMutableDirectedGraph, then you may set the
 * structure to a vtkDirectedGraph using ShallowCopy().
 *
 * @sa
 * vtkGraph vtkMutableDirectedGraph
*/

#ifndef vtkDirectedGraph_h
#define vtkDirectedGraph_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkGraph.h"

class VTKCOMMONDATAMODEL_EXPORT vtkDirectedGraph : public vtkGraph
{
public:
  static vtkDirectedGraph *New();
  vtkTypeMacro(vtkDirectedGraph, vtkGraph);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Return what type of dataset this is.
   */
  int GetDataObjectType() override {return VTK_DIRECTED_GRAPH;}

  //@{
  /**
   * Retrieve a graph from an information vector.
   */
  static vtkDirectedGraph *GetData(vtkInformation *info);
  static vtkDirectedGraph *GetData(vtkInformationVector *v, int i=0);
  //@}

  /**
   * Check the storage, and accept it if it is a valid
   * undirected graph. This is public to allow
   * the ToDirected/UndirectedGraph to work.
   */
  bool IsStructureValid(vtkGraph *g) override;

protected:
  vtkDirectedGraph();
  ~vtkDirectedGraph() override;

private:
  vtkDirectedGraph(const vtkDirectedGraph&) = delete;
  void operator=(const vtkDirectedGraph&) = delete;
};

#endif
