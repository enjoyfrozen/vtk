/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageNormalize
 * @brief   Normalizes that scalar components for each point.
 *
 * For each point, vtkImageNormalize normalizes the vector defined by the
 * scalar components.  If the magnitude of this vector is zero, the output
 * vector is zero also.
*/

#ifndef vtkImageNormalize_h
#define vtkImageNormalize_h


#include "vtkImagingGeneralModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class VTKIMAGINGGENERAL_EXPORT vtkImageNormalize : public vtkThreadedImageAlgorithm
{
public:
  static vtkImageNormalize *New();
  vtkTypeMacro(vtkImageNormalize,vtkThreadedImageAlgorithm);

protected:
  vtkImageNormalize();
  ~vtkImageNormalize() override {}

  int RequestInformation (vtkInformation *, vtkInformationVector**, vtkInformationVector *) override;

  void ThreadedExecute (vtkImageData *inData, vtkImageData *outData,
                       int extent[6], int id) override;
private:
  vtkImageNormalize(const vtkImageNormalize&) = delete;
  void operator=(const vtkImageNormalize&) = delete;
};

#endif










// VTK-HeaderTest-Exclude: vtkImageNormalize.h
