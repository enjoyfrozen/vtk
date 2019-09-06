/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkGraphEdge
 * @brief   Representation of a single graph edge.
 *
 *
 * A heavy-weight (vtkObject subclass) graph edge object that may be used
 * instead of the vtkEdgeType struct, for use with wrappers.
 * The edge contains the source and target vertex ids, and the edge id.
 *
 * @sa
 * vtkGraph
*/

#ifndef vtkGraphEdge_h
#define vtkGraphEdge_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkObject.h"

class VTKCOMMONDATAMODEL_EXPORT vtkGraphEdge : public vtkObject
{
public:
  static vtkGraphEdge *New();
  vtkTypeMacro(vtkGraphEdge, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * The source of the edge.
   */
  vtkSetMacro(Source, vtkIdType);
  vtkGetMacro(Source, vtkIdType);
  //@}

  //@{
  /**
   * The target of the edge.
   */
  vtkSetMacro(Target, vtkIdType);
  vtkGetMacro(Target, vtkIdType);
  //@}

  //@{
  /**
   * The id of the edge.
   */
  vtkSetMacro(Id, vtkIdType);
  vtkGetMacro(Id, vtkIdType);
  //@}

protected:
  vtkGraphEdge();
  ~vtkGraphEdge() override;

  vtkIdType Source;
  vtkIdType Target;
  vtkIdType Id;

private:
  vtkGraphEdge(const vtkGraphEdge&) = delete;
  void operator=(const vtkGraphEdge&) = delete;
};

#endif
