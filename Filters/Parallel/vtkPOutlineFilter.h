/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPOutlineFilter
 * @brief   create wireframe outline for arbitrary data set
 *
 * vtkPOutlineFilter works like vtkOutlineFilter, but it looks for data
 * partitions in other processes.  It assumes the filter is operated
 * in a data parallel pipeline.
*/

#ifndef vtkPOutlineFilter_h
#define vtkPOutlineFilter_h

#include "vtkFiltersParallelModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"
class vtkOutlineSource;
class vtkMultiProcessController;

class VTKFILTERSPARALLEL_EXPORT vtkPOutlineFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkPOutlineFilter *New();
  vtkTypeMacro(vtkPOutlineFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set and get the controller.
   */
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  //@}

protected:
  vtkPOutlineFilter();
  ~vtkPOutlineFilter() override;

  vtkMultiProcessController* Controller;
  vtkOutlineSource *OutlineSource;
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkPOutlineFilter(const vtkPOutlineFilter&) = delete;
  void operator=(const vtkPOutlineFilter&) = delete;
};
#endif
