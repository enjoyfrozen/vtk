/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageFourierCenter
 * @brief   Shifts constant frequency to center for
 * display.
 *
 * Is used for dispaying images in frequency space.  FFT converts spatial
 * images into frequency space, but puts the zero frequency at the origin.
 * This filter shifts the zero frequency to the center of the image.
 * Input and output are assumed to be doubles.
*/

#ifndef vtkImageFourierCenter_h
#define vtkImageFourierCenter_h


#include "vtkImagingFourierModule.h" // For export macro
#include "vtkImageDecomposeFilter.h"

class VTKIMAGINGFOURIER_EXPORT vtkImageFourierCenter : public vtkImageDecomposeFilter
{
public:
  static vtkImageFourierCenter *New();
  vtkTypeMacro(vtkImageFourierCenter,vtkImageDecomposeFilter);

protected:
  vtkImageFourierCenter();
  ~vtkImageFourierCenter() override {}

  int IterativeRequestUpdateExtent(vtkInformation* in,
                                           vtkInformation* out) override;

  void ThreadedRequestData(
    vtkInformation* vtkNotUsed( request ),
    vtkInformationVector** vtkNotUsed( inputVector ),
    vtkInformationVector* outputVector,
    vtkImageData ***inDataVec,
    vtkImageData **outDataVec,
    int outExt[6],
    int threadId) override;
private:
  vtkImageFourierCenter(const vtkImageFourierCenter&) = delete;
  void operator=(const vtkImageFourierCenter&) = delete;
};

#endif










// VTK-HeaderTest-Exclude: vtkImageFourierCenter.h
