/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkCircularLayoutStrategy
 * @brief   Places vertices around a circle
 *
 *
 * Assigns points to the vertices around a circle with unit radius.
*/

#ifndef vtkCircularLayoutStrategy_h
#define vtkCircularLayoutStrategy_h

#include "vtkInfovisLayoutModule.h" // For export macro
#include "vtkGraphLayoutStrategy.h"

class VTKINFOVISLAYOUT_EXPORT vtkCircularLayoutStrategy : public vtkGraphLayoutStrategy
{
public:
  static vtkCircularLayoutStrategy *New();

  vtkTypeMacro(vtkCircularLayoutStrategy, vtkGraphLayoutStrategy);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Perform the layout.
   */
  void Layout() override;

protected:
  vtkCircularLayoutStrategy();
  ~vtkCircularLayoutStrategy() override;

private:
  vtkCircularLayoutStrategy(const vtkCircularLayoutStrategy&) = delete;
  void operator=(const vtkCircularLayoutStrategy&) = delete;
};

#endif

