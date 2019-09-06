/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageImportExecutive
 *
 * vtkImageImportExecutive
*/

#ifndef vtkImageImportExecutive_h
#define vtkImageImportExecutive_h

#include "vtkIOImageModule.h" // For export macro
#include "vtkStreamingDemandDrivenPipeline.h"

class VTKIOIMAGE_EXPORT vtkImageImportExecutive :
  public vtkStreamingDemandDrivenPipeline
{
public:
  static vtkImageImportExecutive* New();
  vtkTypeMacro(vtkImageImportExecutive,
                       vtkStreamingDemandDrivenPipeline);

  /**
   * Override to implement some requests with callbacks.
   */
  int ProcessRequest(vtkInformation* request,
                             vtkInformationVector** inInfo,
                             vtkInformationVector* outInfo) override;

protected:
  vtkImageImportExecutive() {}
  ~vtkImageImportExecutive() override {}

private:
  vtkImageImportExecutive(const vtkImageImportExecutive&) = delete;
  void operator=(const vtkImageImportExecutive&) = delete;
};

#endif
