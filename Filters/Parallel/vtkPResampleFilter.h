/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPResampleFilter
 * @brief   probe dataset in parallel using a vtkImageData
 *
*/

#ifndef vtkPResampleFilter_h
#define vtkPResampleFilter_h

#include "vtkFiltersParallelModule.h" // For export macro
#include "vtkImageAlgorithm.h"

class vtkMultiProcessController;

class VTKFILTERSPARALLEL_EXPORT vtkPResampleFilter : public vtkImageAlgorithm
{
public:
  vtkTypeMacro(vtkPResampleFilter,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkPResampleFilter *New();

  //@{
  /**
   * Set and get the controller.
   */
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  //@}

  //@{
  /**
   * Set/Get if the filter should use Input bounds to sub-sample the data.
   * By default it is set to 1.
   */
  vtkSetMacro(UseInputBounds, vtkTypeBool);
  vtkGetMacro(UseInputBounds, vtkTypeBool);
  vtkBooleanMacro(UseInputBounds, vtkTypeBool);
  //@}

  //@{
  /**
   * Set/Get sampling bounds. If (UseInputBounds == 1) then the sampling
   * bounds won't be used.
   */
  vtkSetVector6Macro(CustomSamplingBounds, double);
  vtkGetVector6Macro(CustomSamplingBounds, double);
  //@}

  //@{
  /**
   * Set/Get sampling dimension along each axis. Default will be [10,10,10]
   */
  vtkSetVector3Macro(SamplingDimension, int);
  vtkGetVector3Macro(SamplingDimension, int);
  //@}

protected:
  vtkPResampleFilter();
  ~vtkPResampleFilter() override;

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int FillInputPortInformation(int port, vtkInformation *info) override;

  double* CalculateBounds(vtkDataSet* input);

  vtkMultiProcessController* Controller;
  vtkTypeBool UseInputBounds;
  double CustomSamplingBounds[6];
  int SamplingDimension[3];
  double Bounds[6];

private:
  vtkPResampleFilter(const vtkPResampleFilter&) = delete;
  void operator=(const vtkPResampleFilter&) = delete;

};

#endif
