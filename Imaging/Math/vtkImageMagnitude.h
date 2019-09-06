/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageMagnitude
 * @brief   Colapses components with magnitude function..
 *
 * vtkImageMagnitude takes the magnitude of the components.
*/

#ifndef vtkImageMagnitude_h
#define vtkImageMagnitude_h


#include "vtkImagingMathModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class VTKIMAGINGMATH_EXPORT vtkImageMagnitude : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageMagnitude *New();
  vtkTypeMacro(vtkImageMagnitude,vtkThreadedImageAlgorithm);

protected:
  vtkImageMagnitude();
  ~vtkImageMagnitude() override {}

  int RequestInformation (vtkInformation *, vtkInformationVector**,
                                  vtkInformationVector *) override;

  void ThreadedExecute (vtkImageData *inData, vtkImageData *outData,
                        int outExt[6], int id) override;

private:
  vtkImageMagnitude(const vtkImageMagnitude&) = delete;
  void operator=(const vtkImageMagnitude&) = delete;
};

#endif










// VTK-HeaderTest-Exclude: vtkImageMagnitude.h
