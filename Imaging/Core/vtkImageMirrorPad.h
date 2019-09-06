/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageMirrorPad
 * @brief   Extra pixels are filled by mirror images.
 *
 * vtkImageMirrorPad makes an image larger by filling extra pixels with
 * a mirror image of the original image (mirror at image boundaries).
*/

#ifndef vtkImageMirrorPad_h
#define vtkImageMirrorPad_h


#include "vtkImagingCoreModule.h" // For export macro
#include "vtkImagePadFilter.h"

class VTKIMAGINGCORE_EXPORT vtkImageMirrorPad : public vtkImagePadFilter
{
public:
  static vtkImageMirrorPad *New();
  vtkTypeMacro(vtkImageMirrorPad,vtkImagePadFilter);

protected:
  vtkImageMirrorPad() {}
  ~vtkImageMirrorPad() override {}

  void ComputeInputUpdateExtent(int inExt[6], int outExt[6], int wExt[6]) override;
  void ThreadedRequestData (vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector,
                            vtkImageData ***inData, vtkImageData **outData,
                            int ext[6], int id) override;
private:
  vtkImageMirrorPad(const vtkImageMirrorPad&) = delete;
  void operator=(const vtkImageMirrorPad&) = delete;
};

#endif



// VTK-HeaderTest-Exclude: vtkImageMirrorPad.h
