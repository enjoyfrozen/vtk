/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSimpleImageFilterExample
 * @brief   Simple example of an image-image filter.
 *
 * This is an example of a simple image-image filter. It copies it's input
 * to it's output (point by point). It shows how templates can be used
 * to support various data types.
 * @sa
 * vtkSimpleImageToImageFilter
*/

#ifndef vtkSimpleImageFilterExample_h
#define vtkSimpleImageFilterExample_h

#include "vtkImagingGeneralModule.h" // For export macro
#include "vtkSimpleImageToImageFilter.h"

class VTKIMAGINGGENERAL_EXPORT vtkSimpleImageFilterExample : public vtkSimpleImageToImageFilter
{
public:
  static vtkSimpleImageFilterExample *New();
  vtkTypeMacro(vtkSimpleImageFilterExample,vtkSimpleImageToImageFilter);

protected:

  vtkSimpleImageFilterExample() {}
  ~vtkSimpleImageFilterExample() override {}

  void SimpleExecute(vtkImageData* input, vtkImageData* output) override;
private:
  vtkSimpleImageFilterExample(const vtkSimpleImageFilterExample&) = delete;
  void operator=(const vtkSimpleImageFilterExample&) = delete;
};

#endif







// VTK-HeaderTest-Exclude: vtkSimpleImageFilterExample.h
