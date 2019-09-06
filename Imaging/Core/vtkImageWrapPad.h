/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageWrapPad
 * @brief   Makes an image larger by wrapping existing data.
 *
 * vtkImageWrapPad performs a modulo operation on the output pixel index
 * to determine the source input index.  The new image extent of the
 * output has to be specified.  Input has to be the same scalar type as
 * output.
*/

#ifndef vtkImageWrapPad_h
#define vtkImageWrapPad_h


#include "vtkImagingCoreModule.h" // For export macro
#include "vtkImagePadFilter.h"

class vtkInformation;
class vtkInformationVector;

class VTKIMAGINGCORE_EXPORT vtkImageWrapPad : public vtkImagePadFilter
{
public:
  static vtkImageWrapPad *New();
  vtkTypeMacro(vtkImageWrapPad,vtkImagePadFilter);

protected:
  vtkImageWrapPad() {}
  ~vtkImageWrapPad() override {}

  void ComputeInputUpdateExtent (int inExt[6], int outExt[6], int wExt[6]) override;
  void ThreadedRequestData (vtkInformation* request,
                            vtkInformationVector** inputVector,
                            vtkInformationVector* outputVector,
                            vtkImageData ***inData, vtkImageData **outData,
                            int ext[6], int id) override;
private:
  vtkImageWrapPad(const vtkImageWrapPad&) = delete;
  void operator=(const vtkImageWrapPad&) = delete;
};

#endif



// VTK-HeaderTest-Exclude: vtkImageWrapPad.h
