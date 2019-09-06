/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPPairwiseExtractHistogram2D
 * @brief   compute a 2D histogram between
 *  all adjacent columns of an input vtkTable in parallel.
 *
 *
 *  This class does exactly the same this as vtkPairwiseExtractHistogram2D,
 *  but does it in a multi-process environment.  After each node
 *  computes their own local histograms, this class does an AllReduce
 *  that distributes the sum of all local histograms onto each node.
 *
 *  Because vtkPairwiseExtractHistogram2D is a light wrapper around a series
 *  of vtkExtractHistogram2D classes, this class just overrides the function
 *  that instantiates new histogram filters and returns the parallel version
 *  (vtkPExtractHistogram2D).
 *
 * @sa
 *  vtkExtractHistogram2D vtkPairwiseExtractHistogram2D vtkPExtractHistogram2D
 *
 * @par Thanks:
 *  Developed by David Feng and Philippe Pebay at Sandia National Laboratories
 *------------------------------------------------------------------------------
*/

#ifndef vtkPPairwiseExtractHistogram2D_h
#define vtkPPairwiseExtractHistogram2D_h

#include "vtkFiltersParallelImagingModule.h" // For export macro
#include "vtkPairwiseExtractHistogram2D.h"

class vtkExtractHistogram2D;
class vtkMultiProcessController;

class VTKFILTERSPARALLELIMAGING_EXPORT vtkPPairwiseExtractHistogram2D : public vtkPairwiseExtractHistogram2D
{
public:
  static vtkPPairwiseExtractHistogram2D* New();
  vtkTypeMacro(vtkPPairwiseExtractHistogram2D, vtkPairwiseExtractHistogram2D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller,vtkMultiProcessController);

protected:
  vtkPPairwiseExtractHistogram2D();
  ~vtkPPairwiseExtractHistogram2D() override;

  vtkMultiProcessController* Controller;

  /**
   * Generate a new histogram filter, but actually generate a parallel one this time.
   */
  vtkExtractHistogram2D* NewHistogramFilter() override;

private:
  vtkPPairwiseExtractHistogram2D(const vtkPPairwiseExtractHistogram2D&) = delete;
  void operator=(const vtkPPairwiseExtractHistogram2D&) = delete;
};

#endif
