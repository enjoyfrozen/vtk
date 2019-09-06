/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageMapToRGBA
 * @brief   map the input image through a lookup table
 *
 * This filter has been replaced by vtkImageMapToColors, which provided
 * additional features.  Use vtkImageMapToColors instead.
 *
 * @sa
 * vtkLookupTable
*/

#ifndef vtkImageMapToRGBA_h
#define vtkImageMapToRGBA_h


#include "vtkImagingColorModule.h" // For export macro
#include "vtkImageMapToColors.h"

class VTKIMAGINGCOLOR_EXPORT vtkImageMapToRGBA : public vtkImageMapToColors
{
public:
  static vtkImageMapToRGBA *New();
  vtkTypeMacro(vtkImageMapToRGBA,vtkImageMapToColors);

protected:
  vtkImageMapToRGBA() {}
  ~vtkImageMapToRGBA() override {}
private:
  vtkImageMapToRGBA(const vtkImageMapToRGBA&) = delete;
  void operator=(const vtkImageMapToRGBA&) = delete;
};

#endif







// VTK-HeaderTest-Exclude: vtkImageMapToRGBA.h
