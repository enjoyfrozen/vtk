/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkVertexListIterator
 * @brief   Iterates all vertices in a graph.
 *
 *
 * vtkVertexListIterator iterates through all vertices in a graph.
 * Create an instance of this and call graph->GetVertices(it) to initialize
 * this iterator. You may alternately call SetGraph() to initialize the
 * iterator.
 *
 * @sa
 * vtkGraph
*/

#ifndef vtkVertexListIterator_h
#define vtkVertexListIterator_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkObject.h"

#include "vtkGraph.h" // For edge type definitions

class vtkGraphEdge;

class VTKCOMMONDATAMODEL_EXPORT vtkVertexListIterator : public vtkObject
{
public:
  static vtkVertexListIterator *New();
  vtkTypeMacro(vtkVertexListIterator, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Setup the iterator with a graph.
   */
  virtual void SetGraph(vtkGraph *graph);

  //@{
  /**
   * Get the graph associated with this iterator.
   */
  vtkGetObjectMacro(Graph, vtkGraph);
  //@}

  //@{
  /**
   * Returns the next edge in the graph.
   */
  vtkIdType Next()
  {
    vtkIdType v = this->Current;
    ++this->Current;
    return v;
  }
  //@}

  /**
   * Whether this iterator has more edges.
   */
  bool HasNext()
  {
    return this->Current != this->End;
  }

protected:
  vtkVertexListIterator();
  ~vtkVertexListIterator() override;

  vtkGraph *Graph;
  vtkIdType  Current;
  vtkIdType  End;

private:
  vtkVertexListIterator(const vtkVertexListIterator&) = delete;
  void operator=(const vtkVertexListIterator&) = delete;
};

#endif
