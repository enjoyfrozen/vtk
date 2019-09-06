/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageLaplacian
 * @brief   Computes divergence of gradient.
 *
 * vtkImageLaplacian computes the Laplacian (like a second derivative)
 * of a scalar image.  The operation is the same as taking the
 * divergence after a gradient.  Boundaries are handled, so the input
 * is the same as the output.
 * Dimensionality determines how the input regions are interpreted.
 * (images, or volumes). The Dimensionality defaults to two.
*/

#ifndef vtkImageLaplacian_h
#define vtkImageLaplacian_h


#include "vtkImagingGeneralModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class VTKIMAGINGGENERAL_EXPORT vtkImageLaplacian : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageLaplacian *New();
  vtkTypeMacro(vtkImageLaplacian,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Determines how the input is interpreted (set of 2d slices ...)
   */
  vtkSetClampMacro(Dimensionality,int,2,3);
  vtkGetMacro(Dimensionality,int);
  //@}

protected:
  vtkImageLaplacian();
  ~vtkImageLaplacian() override {}

  int Dimensionality;

  int RequestUpdateExtent (vtkInformation *, vtkInformationVector **,
    vtkInformationVector *) override;
  void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData, vtkImageData **outData,
                           int outExt[6], int id) override;

private:
  vtkImageLaplacian(const vtkImageLaplacian&) = delete;
  void operator=(const vtkImageLaplacian&) = delete;
};

#endif



