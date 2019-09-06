/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkBoostBiconnectedComponents
 * @brief   Find the biconnected components of a graph
 *
 *
 * The biconnected components of a graph are maximal regions of the graph where
 * the removal of any single vertex from the region will not disconnect the
 * graph.  Every edge belongs to exactly one biconnected component.  The
 * biconnected component of each edge is given in the edge array named
 * "biconnected component".  The biconnected component of each vertex is also
 * given in the vertex array named "biconnected component".  Cut vertices (or
 * articulation points) belong to multiple biconnected components, and break
 * the graph apart if removed.  These are indicated by assigning a component
 * value of -1.  To get the biconnected components that a cut vertex belongs
 * to, traverse its edge list and collect the distinct component ids for its
 * incident edges.
 *
 * Self-loop edges that start and end at the same vertex are not
 * assigned a biconnected component, and are given component id -1.
 *
 * @warning
 * The boost graph bindings currently only support boost version 1.33.1.
 * There are apparently backwards-compatibility issues with later versions.
*/

#ifndef vtkBoostBiconnectedComponents_h
#define vtkBoostBiconnectedComponents_h

#include "vtkInfovisBoostGraphAlgorithmsModule.h" // For export macro
#include "vtkUndirectedGraphAlgorithm.h"

class VTKINFOVISBOOSTGRAPHALGORITHMS_EXPORT vtkBoostBiconnectedComponents : public vtkUndirectedGraphAlgorithm
{
public:
  static vtkBoostBiconnectedComponents *New();
  vtkTypeMacro(vtkBoostBiconnectedComponents, vtkUndirectedGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the output array name. If no output array name is
   * set then the name "biconnected component" is used.
   */
  vtkSetStringMacro(OutputArrayName);
  //@}

protected:
  vtkBoostBiconnectedComponents();
  ~vtkBoostBiconnectedComponents();

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:

  char* OutputArrayName;

  vtkBoostBiconnectedComponents(const vtkBoostBiconnectedComponents&) = delete;
  void operator=(const vtkBoostBiconnectedComponents&) = delete;
};

#endif
