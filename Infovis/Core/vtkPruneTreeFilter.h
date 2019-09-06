/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPruneTreeFilter
 * @brief   prune a subtree out of a vtkTree
 *
 *
 * Removes a subtree rooted at a particular vertex in a vtkTree.
 *
*/

#ifndef vtkPruneTreeFilter_h
#define vtkPruneTreeFilter_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkTreeAlgorithm.h"

class vtkTree;
class vtkPVXMLElement;

class VTKINFOVISCORE_EXPORT vtkPruneTreeFilter : public vtkTreeAlgorithm
{
public:
  static vtkPruneTreeFilter* New();
  vtkTypeMacro(vtkPruneTreeFilter,vtkTreeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the parent vertex of the subtree to remove.
   */
  vtkGetMacro(ParentVertex, vtkIdType);
  vtkSetMacro(ParentVertex, vtkIdType);
  //@}

  //@{
  /**
   * Should we remove the parent vertex, or just its descendants?
   * Default behavior is to remove the parent vertex.
   */
  vtkGetMacro(ShouldPruneParentVertex, bool);
  vtkSetMacro(ShouldPruneParentVertex, bool);
  //@}

protected:
  vtkPruneTreeFilter();
  ~vtkPruneTreeFilter() override;

  vtkIdType ParentVertex;
  bool ShouldPruneParentVertex;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

private:
  vtkPruneTreeFilter(const vtkPruneTreeFilter&) = delete;
  void operator=(const vtkPruneTreeFilter&) = delete;
};

#endif

