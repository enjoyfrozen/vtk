/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageSobel3D
 * @brief   Computes a vector field using sobel functions.
 *
 * vtkImageSobel3D computes a vector field from a scalar field by using
 * Sobel functions.  The number of vector components is 3 because
 * the input is a volume.  Output is always doubles.  A little creative
 * liberty was used to extend the 2D sobel kernels into 3D.
*/

#ifndef vtkImageSobel3D_h
#define vtkImageSobel3D_h


#include "vtkImagingGeneralModule.h" // For export macro
#include "vtkImageSpatialAlgorithm.h"

class VTKIMAGINGGENERAL_EXPORT vtkImageSobel3D : public vtkImageSpatialAlgorithm
{
public:
  static vtkImageSobel3D *New();
  vtkTypeMacro(vtkImageSobel3D,vtkImageSpatialAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkImageSobel3D();
  ~vtkImageSobel3D() override {}

  void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData, vtkImageData **outData,
                           int outExt[6], int id) override;
  int RequestInformation (vtkInformation *request,
                                  vtkInformationVector **inputVector,
                                  vtkInformationVector *outputVector) override;

private:
  vtkImageSobel3D(const vtkImageSobel3D&) = delete;
  void operator=(const vtkImageSobel3D&) = delete;
};

#endif
