/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPassThroughEdgeStrategy
 * @brief   passes edge routing information through
 *
 *
 * Simply passes existing edge layout information from the input to the
 * output without making changes.
*/

#ifndef vtkPassThroughEdgeStrategy_h
#define vtkPassThroughEdgeStrategy_h

#include "vtkInfovisLayoutModule.h" // For export macro
#include "vtkEdgeLayoutStrategy.h"

class VTKINFOVISLAYOUT_EXPORT vtkPassThroughEdgeStrategy : public vtkEdgeLayoutStrategy
{
public:
  static vtkPassThroughEdgeStrategy* New();
  vtkTypeMacro(vtkPassThroughEdgeStrategy,vtkEdgeLayoutStrategy);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * This is the layout method where the graph that was
   * set in SetGraph() is laid out.
   */
  void Layout() override;

protected:
  vtkPassThroughEdgeStrategy();
  ~vtkPassThroughEdgeStrategy() override;

private:
  vtkPassThroughEdgeStrategy(const vtkPassThroughEdgeStrategy&) = delete;
  void operator=(const vtkPassThroughEdgeStrategy&) = delete;
};

#endif

