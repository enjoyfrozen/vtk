/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkReaderExecutive
 * @brief   Executive that works with vtkReaderAlgorithm and subclasses.
 *
 * vtkReaderExecutive is an executive that supports simplified API readers
 * that are written by subclassing from the vtkReaderAlgorithm hierarchy.
 * Currently, its main functionality is to call the basic reader API instead
 * if the standard ProcessRequest() method that other algorithms use.
 * In time, this is likely to add functionality such as caching. See
 * vtkReaderAlgorithm for the API.
 *
 * Note that this executive assumes that the reader has one output port.
*/

#ifndef vtkReaderExecutive_h
#define vtkReaderExecutive_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkStreamingDemandDrivenPipeline.h"

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkReaderExecutive :
  public vtkStreamingDemandDrivenPipeline
{
public:
  static vtkReaderExecutive* New();
  vtkTypeMacro(vtkReaderExecutive,vtkStreamingDemandDrivenPipeline);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Overwritten to call the vtkReaderAlgorithm API instead of
   * ProcessRequest().
   */
  virtual int CallAlgorithm(vtkInformation* request, int direction,
                            vtkInformationVector** inInfo,
                            vtkInformationVector* outInfo) override;

protected:
  vtkReaderExecutive();
  ~vtkReaderExecutive() override;

private:
  vtkReaderExecutive(const vtkReaderExecutive&) = delete;
  void operator=(const vtkReaderExecutive&) = delete;
};

#endif
