/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPDescriptiveStatistics
 * @brief   A class for parallel univariate descriptive statistics
 *
 * vtkPDescriptiveStatistics is vtkDescriptiveStatistics subclass for parallel datasets.
 * It learns and derives the global statistical model on each node, but assesses each
 * individual data points on the node that owns it.
 *
 * @par Thanks:
 * Thanks to Philippe Pebay from Sandia National Laboratories for implementing this class.
*/

#ifndef vtkPDescriptiveStatistics_h
#define vtkPDescriptiveStatistics_h

#include "vtkFiltersParallelStatisticsModule.h" // For export macro
#include "vtkDescriptiveStatistics.h"

class vtkMultiBlockDataSet;
class vtkMultiProcessController;

class VTKFILTERSPARALLELSTATISTICS_EXPORT vtkPDescriptiveStatistics : public vtkDescriptiveStatistics
{
public:
  static vtkPDescriptiveStatistics* New();
  vtkTypeMacro(vtkPDescriptiveStatistics, vtkDescriptiveStatistics);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get/Set the multiprocess controller. If no controller is set,
   * single process is assumed.
   */
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  //@}

  /**
   * Execute the parallel calculations required by the Learn option.
   */
  void Learn( vtkTable* inData,
              vtkTable* inParameters,
              vtkMultiBlockDataSet* outMeta ) override;

protected:
  vtkPDescriptiveStatistics();
  ~vtkPDescriptiveStatistics() override;

  vtkMultiProcessController* Controller;
private:
  vtkPDescriptiveStatistics(const vtkPDescriptiveStatistics&) = delete;
  void operator=(const vtkPDescriptiveStatistics&) = delete;
};

#endif
