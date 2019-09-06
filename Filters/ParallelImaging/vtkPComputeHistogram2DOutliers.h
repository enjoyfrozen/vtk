/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkPComputeHistogram2DOutliers
 * @brief   extract outlier rows from
 *  a vtkTable based on input 2D histograms, in parallel.
 *
 *
 *  This class does exactly the same this as vtkComputeHistogram2DOutliers,
 *  but does it in a multi-process environment.  After each node
 *  computes their own local outliers, class does an AllGather
 *  that distributes the outliers to every node.  This could probably just
 *  be a Gather onto the root node instead.
 *
 *  After this operation, the row selection will only contain local row ids,
 *  since I'm not sure how to deal with distributed ids.
 *
 * @sa
 *  vtkComputeHistogram2DOutliers
 *
 * @par Thanks:
 *  Developed by David Feng at Sandia National Laboratories
 *------------------------------------------------------------------------------
*/

#ifndef vtkPComputeHistogram2DOutliers_h
#define vtkPComputeHistogram2DOutliers_h
//------------------------------------------------------------------------------
#include "vtkFiltersParallelImagingModule.h" // For export macro
#include "vtkComputeHistogram2DOutliers.h"
//------------------------------------------------------------------------------
class vtkMultiProcessController;
//------------------------------------------------------------------------------
class VTKFILTERSPARALLELIMAGING_EXPORT vtkPComputeHistogram2DOutliers : public vtkComputeHistogram2DOutliers
{
public:
  static vtkPComputeHistogram2DOutliers* New();
  vtkTypeMacro(vtkPComputeHistogram2DOutliers, vtkComputeHistogram2DOutliers);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller,vtkMultiProcessController);
protected:
  vtkPComputeHistogram2DOutliers();
  ~vtkPComputeHistogram2DOutliers() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  vtkMultiProcessController* Controller;
private:
  vtkPComputeHistogram2DOutliers(const vtkPComputeHistogram2DOutliers&) = delete;
  void operator=(const vtkPComputeHistogram2DOutliers&) = delete;
};

#endif
