/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageRGBToYIQ
 * @brief   Converts YIQ components to RGB.
 *
 * For each pixel with Y, I, and Q components this
 * filter output the color coded as RGB.
 * Output type must be the same as input type.
*/

#ifndef vtkImageYIQToRGB_h
#define vtkImageYIQToRGB_h


#include "vtkImagingColorModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class VTKIMAGINGCOLOR_EXPORT vtkImageYIQToRGB : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageYIQToRGB *New();
  vtkTypeMacro(vtkImageYIQToRGB,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetMacro(Maximum,double);
  vtkGetMacro(Maximum,double);

protected:
  vtkImageYIQToRGB();
  ~vtkImageYIQToRGB() override {}

  double Maximum; // Maximum value of pixel intensity allowed

  void ThreadedExecute (vtkImageData *inData, vtkImageData *outData,
                       int ext[6], int id) override;
private:
  vtkImageYIQToRGB(const vtkImageYIQToRGB&) = delete;
  void operator=(const vtkImageYIQToRGB&) = delete;
};

#endif
