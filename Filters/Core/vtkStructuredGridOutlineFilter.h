/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkStructuredGridOutlineFilter
 * @brief   create wireframe outline for structured grid
 *
 * vtkStructuredGridOutlineFilter is a filter that generates a wireframe
 * outline of a structured grid (vtkStructuredGrid). Structured data is
 * topologically a cube, so the outline will have 12 "edges".
*/

#ifndef vtkStructuredGridOutlineFilter_h
#define vtkStructuredGridOutlineFilter_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSCORE_EXPORT vtkStructuredGridOutlineFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkStructuredGridOutlineFilter *New();
  vtkTypeMacro(vtkStructuredGridOutlineFilter,vtkPolyDataAlgorithm);

protected:
  vtkStructuredGridOutlineFilter() {}
  ~vtkStructuredGridOutlineFilter() override {}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkStructuredGridOutlineFilter(const vtkStructuredGridOutlineFilter&) = delete;
  void operator=(const vtkStructuredGridOutlineFilter&) = delete;
};

#endif
// VTK-HeaderTest-Exclude: vtkStructuredGridOutlineFilter.h
