/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHierarchicalDataLevelFilter
 * @brief   generate scalars from levels
 *
 * Legacy class. Use vtkLevelIdScalars instead.
 *
 * @sa
 * vtkLevelIdScalars
*/

#ifndef vtkHierarchicalDataLevelFilter_h
#define vtkHierarchicalDataLevelFilter_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkLevelIdScalars.h"

class VTKFILTERSGENERAL_EXPORT vtkHierarchicalDataLevelFilter : public vtkLevelIdScalars
{
public:
  vtkTypeMacro(vtkHierarchicalDataLevelFilter,vtkLevelIdScalars);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Construct object with PointIds and CellIds on; and ids being generated
   * as scalars.
   */
  static vtkHierarchicalDataLevelFilter *New();

protected:
  vtkHierarchicalDataLevelFilter();
  ~vtkHierarchicalDataLevelFilter() override;

private:
  vtkHierarchicalDataLevelFilter(const vtkHierarchicalDataLevelFilter&) = delete;
  void operator=(const vtkHierarchicalDataLevelFilter&) = delete;
};

#endif


