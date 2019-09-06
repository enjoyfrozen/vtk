/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPBivariateLinearTableThreshold
 * @brief   performs line-based thresholding
 * for vtkTable data in parallel.
 *
 *
 * Perform the table filtering operations provided by
 * vtkBivariateLinearTableThreshold in parallel.
*/

#ifndef vtkPBivariateLinearTableThreshold_h
#define vtkPBivariateLinearTableThreshold_h

#include "vtkFiltersParallelStatisticsModule.h" // For export macro
#include "vtkBivariateLinearTableThreshold.h"

class vtkIdTypeArray;
class vtkMultiProcessController;

class VTKFILTERSPARALLELSTATISTICS_EXPORT vtkPBivariateLinearTableThreshold : public vtkBivariateLinearTableThreshold
{
public:
  static vtkPBivariateLinearTableThreshold* New();
  vtkTypeMacro(vtkPBivariateLinearTableThreshold, vtkBivariateLinearTableThreshold);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the vtkMultiProcessController to be used for combining filter
   * results from the individual nodes.
   */
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller,vtkMultiProcessController);
  //@}

protected:
  vtkPBivariateLinearTableThreshold();
  ~vtkPBivariateLinearTableThreshold() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  vtkMultiProcessController* Controller;
private:
  vtkPBivariateLinearTableThreshold(const vtkPBivariateLinearTableThreshold&) = delete;
  void operator=(const vtkPBivariateLinearTableThreshold&) = delete;
};

#endif
