/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageDotProduct
 * @brief   Dot product of two vector images.
 *
 * vtkImageDotProduct interprets the scalar components of two images
 * as vectors and takes the dot product vector by vector (pixel by pixel).
*/

#ifndef vtkImageDotProduct_h
#define vtkImageDotProduct_h



#include "vtkImagingMathModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class VTKIMAGINGMATH_EXPORT vtkImageDotProduct : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageDotProduct *New();
  vtkTypeMacro(vtkImageDotProduct,vtkThreadedImageAlgorithm);

  /**
   * Set the two inputs to this filter
   */
  virtual void SetInput1Data(vtkDataObject *in) { this->SetInputData(0,in); }
  virtual void SetInput2Data(vtkDataObject *in) { this->SetInputData(1,in); }

protected:
  vtkImageDotProduct();
  ~vtkImageDotProduct() override {}

  int RequestInformation (vtkInformation *,
                                  vtkInformationVector **,
                                  vtkInformationVector *) override;

  void ThreadedRequestData(vtkInformation *request,
                                   vtkInformationVector **inputVector,
                                   vtkInformationVector *outputVector,
                                   vtkImageData ***inData,
                                   vtkImageData **outData,
                                   int extent[6], int threadId) override;

private:
  vtkImageDotProduct(const vtkImageDotProduct&) = delete;
  void operator=(const vtkImageDotProduct&) = delete;
};

#endif



// VTK-HeaderTest-Exclude: vtkImageDotProduct.h
