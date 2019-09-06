/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkRectilinearGridOutlineFilter
 * @brief   create wireframe outline for a rectilinear grid.
 *
 * vtkRectilinearGridOutlineFilter works in parallel.  There is no reason.
 * to use this filter if you are not breaking the processing into pieces.
 * With one piece you can simply use vtkOutlineFilter.  This filter
 * ignores internal edges when the extent is not the whole extent.
*/

#ifndef vtkRectilinearGridOutlineFilter_h
#define vtkRectilinearGridOutlineFilter_h

#include "vtkFiltersParallelModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSPARALLEL_EXPORT vtkRectilinearGridOutlineFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkRectilinearGridOutlineFilter *New();
  vtkTypeMacro(vtkRectilinearGridOutlineFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkRectilinearGridOutlineFilter() {}
  ~vtkRectilinearGridOutlineFilter() override {}
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkRectilinearGridOutlineFilter(const vtkRectilinearGridOutlineFilter&) = delete;
  void operator=(const vtkRectilinearGridOutlineFilter&) = delete;
};

#endif
