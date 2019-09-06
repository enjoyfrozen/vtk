/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHierarchicalDataSetGeometryFilter
 * @brief   extract geometry from hierarchical data
 *
 * Legacy class. Use vtkCompositeDataGeometryFilter instead.
 *
 * @sa
 * vtkCompositeDataGeometryFilter
*/

#ifndef vtkHierarchicalDataSetGeometryFilter_h
#define vtkHierarchicalDataSetGeometryFilter_h

#include "vtkFiltersGeometryModule.h" // For export macro
#include "vtkCompositeDataGeometryFilter.h"

class vtkPolyData;

class VTKFILTERSGEOMETRY_EXPORT vtkHierarchicalDataSetGeometryFilter :
  public vtkCompositeDataGeometryFilter
{
public:
  static vtkHierarchicalDataSetGeometryFilter *New();
  vtkTypeMacro(vtkHierarchicalDataSetGeometryFilter,
    vtkCompositeDataGeometryFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override;


protected:
  vtkHierarchicalDataSetGeometryFilter();
  ~vtkHierarchicalDataSetGeometryFilter() override;

private:
  vtkHierarchicalDataSetGeometryFilter(const vtkHierarchicalDataSetGeometryFilter&) = delete;
  void operator=(const vtkHierarchicalDataSetGeometryFilter&) = delete;
};

#endif


