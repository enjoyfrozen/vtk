/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkQImageToImageSource
 * @brief   Create image data from a QImage.
 *
 * vtkQImageToImageSource produces image data from a QImage.
*/

#ifndef vtkQImageToImageSource_h
#define vtkQImageToImageSource_h

#include "vtkRenderingQtModule.h" // For export macro
#include "vtkImageAlgorithm.h"

class QImage;

class VTKRENDERINGQT_EXPORT vtkQImageToImageSource : public vtkImageAlgorithm
{
public:
  static vtkQImageToImageSource *New();
  vtkTypeMacro(vtkQImageToImageSource,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Set/Get QImage surface to be used.
   */
  void SetQImage( QImage* image )
      {this->QtImage = image; this->Modified();}
  const QImage* GetQImage(){return QtImage;}

protected:
  vtkQImageToImageSource();
  ~vtkQImageToImageSource() override {}

  const QImage* QtImage;
  int DataExtent[6];

  int RequestData( vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestInformation ( vtkInformation * vtkNotUsed(request),
                           vtkInformationVector ** vtkNotUsed( inputVector ),
                           vtkInformationVector *outputVector) override;
private:
  vtkQImageToImageSource(const vtkQImageToImageSource&) = delete;
  void operator=(const vtkQImageToImageSource&) = delete;
};


#endif
