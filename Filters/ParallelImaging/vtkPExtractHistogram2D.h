/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPExtractHistogram2D
 * @brief   compute a 2D histogram between two columns
 *  of an input vtkTable in parallel.
 *
 *
 *  This class does exactly the same this as vtkExtractHistogram2D,
 *  but does it in a multi-process environment.  After each node
 *  computes their own local histograms, this class does an AllReduce
 *  that distributes the sum of all local histograms onto each node.
 *
 * @sa
 *  vtkExtractHistogram2D
 *
 * @par Thanks:
 *  Developed by David Feng and Philippe Pebay at Sandia National Laboratories
 *------------------------------------------------------------------------------
*/

#ifndef vtkPExtractHistogram2D_h
#define vtkPExtractHistogram2D_h

#include "vtkFiltersParallelImagingModule.h" // For export macro
#include "vtkExtractHistogram2D.h"

class vtkMultiBlockDataSet;
class vtkMultiProcessController;

class VTKFILTERSPARALLELIMAGING_EXPORT vtkPExtractHistogram2D : public vtkExtractHistogram2D
{
public:
  static vtkPExtractHistogram2D* New();
  vtkTypeMacro(vtkPExtractHistogram2D, vtkExtractHistogram2D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller,vtkMultiProcessController);

protected:
  vtkPExtractHistogram2D();
  ~vtkPExtractHistogram2D() override;

  vtkMultiProcessController* Controller;

  int ComputeBinExtents(vtkDataArray* col1, vtkDataArray* col2) override;

  // Execute the calculations required by the Learn option.
  void Learn( vtkTable* inData,
                      vtkTable* inParameters,
                      vtkMultiBlockDataSet* outMeta ) override;

private:
  vtkPExtractHistogram2D(const vtkPExtractHistogram2D&) = delete;
  void operator=(const vtkPExtractHistogram2D&) = delete;
};

#endif
