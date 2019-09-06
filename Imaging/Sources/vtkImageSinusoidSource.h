/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageSinusoidSource
 * @brief   Create an image with sinusoidal pixel values.
 *
 * vtkImageSinusoidSource just produces images with pixel values determined
 * by a sinusoid.
*/

#ifndef vtkImageSinusoidSource_h
#define vtkImageSinusoidSource_h

#include "vtkImagingSourcesModule.h" // For export macro
#include "vtkImageAlgorithm.h"

class VTKIMAGINGSOURCES_EXPORT vtkImageSinusoidSource : public vtkImageAlgorithm
{
public:
  static vtkImageSinusoidSource *New();
  vtkTypeMacro(vtkImageSinusoidSource,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Set/Get the extent of the whole output image.
   */
  void SetWholeExtent(int xMinx, int xMax, int yMin, int yMax,
                      int zMin, int zMax);

  //@{
  /**
   * Set/Get the direction vector which determines the sinusoidal
   * orientation. The magnitude is ignored.
   */
  void SetDirection(double,double,double);
  void SetDirection(double dir[3]);
  vtkGetVector3Macro(Direction, double);
  //@}

  //@{
  /**
   * Set/Get the period of the sinusoid in pixels.
   */
  vtkSetMacro(Period, double);
  vtkGetMacro(Period, double);
  //@}

  //@{
  /**
   * Set/Get the phase: 0->2Pi.  0 => Cosine, pi/2 => Sine.
   */
  vtkSetMacro(Phase, double);
  vtkGetMacro(Phase, double);
  //@}

  //@{
  /**
   * Set/Get the magnitude of the sinusoid.
   */
  vtkSetMacro(Amplitude, double);
  vtkGetMacro(Amplitude, double);
  //@}

protected:
  vtkImageSinusoidSource();
  ~vtkImageSinusoidSource() override {}

  int WholeExtent[6];
  double Direction[3];
  double Period;
  double Phase;
  double Amplitude;

  int RequestInformation (vtkInformation *, vtkInformationVector**, vtkInformationVector *) override;
  void ExecuteDataWithInformation(vtkDataObject *data, vtkInformation* outInfo) override;
private:
  vtkImageSinusoidSource(const vtkImageSinusoidSource&) = delete;
  void operator=(const vtkImageSinusoidSource&) = delete;
};


#endif



