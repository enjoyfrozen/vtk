/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageLuminance
 * @brief   Computes the luminance of the input
 *
 * vtkImageLuminance calculates luminance from an rgb input.
*/

#ifndef vtkImageLuminance_h
#define vtkImageLuminance_h


#include "vtkImagingColorModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class VTKIMAGINGCOLOR_EXPORT vtkImageLuminance : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageLuminance *New();
  vtkTypeMacro(vtkImageLuminance,vtkThreadedImageAlgorithm);

protected:
  vtkImageLuminance();
  ~vtkImageLuminance() override {}

  int RequestInformation (vtkInformation *, vtkInformationVector**,
                                  vtkInformationVector *) override;

  void ThreadedExecute (vtkImageData *inData, vtkImageData *outData,
                        int outExt[6], int id) override;

private:
  vtkImageLuminance(const vtkImageLuminance&) = delete;
  void operator=(const vtkImageLuminance&) = delete;
};

#endif










// VTK-HeaderTest-Exclude: vtkImageLuminance.h
