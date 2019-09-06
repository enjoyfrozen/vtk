/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkAggregateDataSetFilter
 * @brief   Aggregates data sets to a reduced number of processes.
 *
 * This class allows polydata and unstructured grids to be aggregated
 * over a smaller set of processes. The derived vtkDIYAggregateDataSetFilter
 * will operate on image data, rectilinear grids and structured grids.
*/

#ifndef vtkAggregateDataSetFilter_h
#define vtkAggregateDataSetFilter_h

#include "vtkFiltersParallelModule.h" // For export macro
#include "vtkPassInputTypeAlgorithm.h"

class vtkDataSet;
class vtkMultiProcessController;

class VTKFILTERSPARALLEL_EXPORT vtkAggregateDataSetFilter : public vtkPassInputTypeAlgorithm
{
public:
  static vtkAggregateDataSetFilter* New();
  vtkTypeMacro(vtkAggregateDataSetFilter, vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Number of target processes. Valid values are between 1 and the total
   * number of processes. The default is 1. If a value is passed in that
   * is less than 1 than NumberOfTargetProcesses is changed/kept at 1.
   * If a value is passed in that is greater than the total number of
   * processes then NumberOfTargetProcesses is changed/kept at the
   * total number of processes. This is useful for scripting use cases
   * where later on the script is run with more processes than the
   * current amount.
   */
  void SetNumberOfTargetProcesses(int);
  vtkGetMacro(NumberOfTargetProcesses, int);
  //@}

protected:
  vtkAggregateDataSetFilter();
  ~vtkAggregateDataSetFilter() override;

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

  int NumberOfTargetProcesses;

private:
  vtkAggregateDataSetFilter(const vtkAggregateDataSetFilter&) = delete;
  void operator=(const vtkAggregateDataSetFilter&) = delete;
};

#endif
