/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageTranslateExtent
 * @brief   Changes extent, nothing else.
 *
 * vtkImageTranslateExtent shift the whole extent, but does not
 * change the data.
*/

#ifndef vtkImageTranslateExtent_h
#define vtkImageTranslateExtent_h

#include "vtkImagingCoreModule.h" // For export macro
#include "vtkImageAlgorithm.h"

class VTKIMAGINGCORE_EXPORT vtkImageTranslateExtent : public vtkImageAlgorithm
{
public:
  static vtkImageTranslateExtent *New();
  vtkTypeMacro(vtkImageTranslateExtent,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Delta to change "WholeExtent". -1 changes 0->10 to -1->9.
   */
  vtkSetVector3Macro(Translation, int);
  vtkGetVector3Macro(Translation, int);
  //@}

protected:
  vtkImageTranslateExtent();
  ~vtkImageTranslateExtent() override {}

  int Translation[3];

  int RequestUpdateExtent (vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int RequestInformation (vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
  vtkImageTranslateExtent(const vtkImageTranslateExtent&) = delete;
  void operator=(const vtkImageTranslateExtent&) = delete;
};

#endif
