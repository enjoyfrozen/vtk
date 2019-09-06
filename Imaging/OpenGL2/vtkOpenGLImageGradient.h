/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOpenGLImageGradient
 * @brief   Compute Gradient using the GPU
*/

#ifndef vtkOpenGLImageGradient_h
#define vtkOpenGLImageGradient_h

#include "vtkImagingOpenGL2Module.h" // For export macro
#include "vtkImageGradient.h"

class vtkOpenGLImageAlgorithmHelper;
class vtkRenderWindow;

class VTKIMAGINGOPENGL2_EXPORT vtkOpenGLImageGradient : public vtkImageGradient
{
public:
  static vtkOpenGLImageGradient *New();
  vtkTypeMacro(vtkOpenGLImageGradient,vtkImageGradient);

  /**
   * Set the render window to get the OpenGL resources from
   */
  void SetRenderWindow(vtkRenderWindow *);

protected:
  void PrintSelf(ostream& os, vtkIndent indent) override;
  vtkOpenGLImageGradient();
  ~vtkOpenGLImageGradient();

  vtkOpenGLImageAlgorithmHelper *Helper;

  void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData, vtkImageData **outData,
                           int extent[6], int id) override;

private:
  vtkOpenGLImageGradient(const vtkOpenGLImageGradient&) = delete;
  void operator=(const vtkOpenGLImageGradient&) = delete;
};

#endif
