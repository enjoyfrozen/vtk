/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPMultiCorrelativeStatistics
 * @brief   A class for parallel bivariate correlative statistics
 *
 * vtkPMultiCorrelativeStatistics is vtkMultiCorrelativeStatistics subclass for parallel datasets.
 * It learns and derives the global statistical model on each node, but assesses each
 * individual data points on the node that owns it.
 *
 * @par Thanks:
 * Thanks to Philippe Pebay and David Thompson from Sandia National Laboratories for implementing this class.
*/

#ifndef vtkPMultiCorrelativeStatistics_h
#define vtkPMultiCorrelativeStatistics_h

#include "vtkFiltersParallelStatisticsModule.h" // For export macro
#include "vtkMultiCorrelativeStatistics.h"

class vtkMultiProcessController;

class VTKFILTERSPARALLELSTATISTICS_EXPORT vtkPMultiCorrelativeStatistics : public vtkMultiCorrelativeStatistics
{
public:
  static vtkPMultiCorrelativeStatistics* New();
  vtkTypeMacro(vtkPMultiCorrelativeStatistics, vtkMultiCorrelativeStatistics);
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
   * Performs Reduction
   */
  static void GatherStatistics( vtkMultiProcessController *curController,
                                vtkTable *sparseCov );

protected:
  vtkPMultiCorrelativeStatistics();
  ~vtkPMultiCorrelativeStatistics() override;

  vtkMultiProcessController* Controller;

  // Execute the parallel calculations required by the Learn option.
  void Learn( vtkTable* inData,
              vtkTable* inParameters,
              vtkMultiBlockDataSet* outMeta ) override;

  vtkOrderStatistics* CreateOrderStatisticsInstance() override;

private:
  vtkPMultiCorrelativeStatistics(const vtkPMultiCorrelativeStatistics&) = delete;
  void operator=(const vtkPMultiCorrelativeStatistics&) = delete;
};

#endif
