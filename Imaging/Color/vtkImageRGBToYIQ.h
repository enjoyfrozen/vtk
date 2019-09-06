/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageRGBToYIQ
 * @brief   Converts RGB components to YIQ.
 *
 * For each pixel with red, blue, and green components this
 * filter output the color coded as YIQ.
 * Output type must be the same as input type.
*/

#ifndef vtkImageRGBToYIQ_h
#define vtkImageRGBToYIQ_h


#include "vtkImagingColorModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class VTKIMAGINGCOLOR_EXPORT vtkImageRGBToYIQ : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageRGBToYIQ *New();
  vtkTypeMacro(vtkImageRGBToYIQ,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetMacro(Maximum,double);
  vtkGetMacro(Maximum,double);

protected:
  vtkImageRGBToYIQ();
  ~vtkImageRGBToYIQ() override {}

  double Maximum;      // Maximum value of pixel intensity allowed

  void ThreadedExecute (vtkImageData *inData, vtkImageData *outData,
                       int ext[6], int id) override;
private:
  vtkImageRGBToYIQ(const vtkImageRGBToYIQ&) = delete;
  void operator=(const vtkImageRGBToYIQ&) = delete;
};

#endif
