/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPCorrelativeStatistics
 * @brief   A class for parallel bivariate correlative statistics
 *
 * vtkPCorrelativeStatistics is vtkCorrelativeStatistics subclass for parallel datasets.
 * It learns and derives the global statistical model on each node, but assesses each
 * individual data points on the node that owns it.
 *
 * @par Thanks:
 * Thanks to Philippe Pebay from Sandia National Laboratories for implementing this class.
*/

#ifndef vtkPCorrelativeStatistics_h
#define vtkPCorrelativeStatistics_h

#include "vtkFiltersParallelStatisticsModule.h" // For export macro
#include "vtkCorrelativeStatistics.h"

class vtkMultiBlockDataSet;
class vtkMultiProcessController;

class VTKFILTERSPARALLELSTATISTICS_EXPORT vtkPCorrelativeStatistics : public vtkCorrelativeStatistics
{
public:
  static vtkPCorrelativeStatistics* New();
  vtkTypeMacro(vtkPCorrelativeStatistics, vtkCorrelativeStatistics);
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

  /**
   * Execute the calculations required by the Test option.
   * NB: Not implemented for more than 1 processor
   */
  void Test( vtkTable*,
             vtkMultiBlockDataSet*,
             vtkTable* ) override;

protected:
  vtkPCorrelativeStatistics();
  ~vtkPCorrelativeStatistics() override;

  vtkMultiProcessController* Controller;
private:
  vtkPCorrelativeStatistics(const vtkPCorrelativeStatistics&) = delete;
  void operator=(const vtkPCorrelativeStatistics&) = delete;
};

#endif
