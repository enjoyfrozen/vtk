/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkBoxLayoutStrategy
 * @brief   a tree map layout that puts vertices in square-ish boxes
 *
 *
 * vtkBoxLayoutStrategy recursively partitions the space for children vertices
 * in a tree-map into square regions (or regions very close to a square).
 *
 * @par Thanks:
 * Thanks to Brian Wylie from Sandia National Laboratories for creating this class.
*/

#ifndef vtkBoxLayoutStrategy_h
#define vtkBoxLayoutStrategy_h

#include "vtkInfovisLayoutModule.h" // For export macro
#include "vtkTreeMapLayoutStrategy.h"

class VTKINFOVISLAYOUT_EXPORT vtkBoxLayoutStrategy : public vtkTreeMapLayoutStrategy
{
public:
  static vtkBoxLayoutStrategy *New();

  vtkTypeMacro(vtkBoxLayoutStrategy,vtkTreeMapLayoutStrategy);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Perform the layout of a tree and place the results as 4-tuples in
   * coordsArray (Xmin, Xmax, Ymin, Ymax).
   */
  void Layout(
      vtkTree* inputTree,
      vtkDataArray* coordsArray,
      vtkDataArray* sizeArray) override;

protected:
  vtkBoxLayoutStrategy();
  ~vtkBoxLayoutStrategy() override;

private:

  void LayoutChildren(vtkTree *inputTree, vtkDataArray *coordsArray,
    vtkIdType parentId,
    float parentMinX, float parentMaxX,
    float parentMinY, float parentMaxY);

  vtkBoxLayoutStrategy(const vtkBoxLayoutStrategy&) = delete;
  void operator=(const vtkBoxLayoutStrategy&) = delete;
};

#endif

