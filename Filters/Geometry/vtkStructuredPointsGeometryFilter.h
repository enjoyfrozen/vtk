/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkStructuredPointsGeometryFilter
 * @brief   obsolete class
 *
 * vtkStructuredPointsGeometryFilter has been renamed to
 * vtkImageDataGeometryFilter
*/

#ifndef vtkStructuredPointsGeometryFilter_h
#define vtkStructuredPointsGeometryFilter_h

#include "vtkFiltersGeometryModule.h" // For export macro
#include "vtkImageDataGeometryFilter.h"

class VTKFILTERSGEOMETRY_EXPORT vtkStructuredPointsGeometryFilter : public vtkImageDataGeometryFilter
{
public:
  vtkTypeMacro(vtkStructuredPointsGeometryFilter,vtkImageDataGeometryFilter);

  /**
   * Construct with initial extent of all the data
   */
  static vtkStructuredPointsGeometryFilter *New();

protected:
  vtkStructuredPointsGeometryFilter();
  ~vtkStructuredPointsGeometryFilter() override {}

private:
  vtkStructuredPointsGeometryFilter(const vtkStructuredPointsGeometryFilter&) = delete;
  void operator=(const vtkStructuredPointsGeometryFilter&) = delete;
};

#endif
// VTK-HeaderTest-Exclude: vtkStructuredPointsGeometryFilter.h
