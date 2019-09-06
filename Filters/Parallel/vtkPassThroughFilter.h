/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPassThroughFilter
 * @brief   Filter which shallow copies it's input to it's output
 *
 * This filter shallow copies it's input to it's output. It is normally
 * used by PVSources with multiple outputs as the VTK filter in the
 * dummy connection objects at each output.
*/

#ifndef vtkPassThroughFilter_h
#define vtkPassThroughFilter_h

#include "vtkFiltersParallelModule.h" // For export macro
#include "vtkDataSetAlgorithm.h"

class vtkFieldData;

class VTKFILTERSPARALLEL_EXPORT vtkPassThroughFilter : public vtkDataSetAlgorithm
{
public:
  vtkTypeMacro(vtkPassThroughFilter,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Create a new vtkPassThroughFilter.
   */
  static vtkPassThroughFilter *New();


protected:

  vtkPassThroughFilter() {}
  ~vtkPassThroughFilter() override {}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
  vtkPassThroughFilter(const vtkPassThroughFilter&) = delete;
  void operator=(const vtkPassThroughFilter&) = delete;
};

#endif


