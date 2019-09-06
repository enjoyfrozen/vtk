/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCachedStreamingDemandDrivenPipeline
 *
 * vtkCachedStreamingDemandDrivenPipeline
*/

#ifndef vtkCachedStreamingDemandDrivenPipeline_h
#define vtkCachedStreamingDemandDrivenPipeline_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkStreamingDemandDrivenPipeline.h"

class vtkInformationIntegerKey;
class vtkInformationIntegerVectorKey;

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkCachedStreamingDemandDrivenPipeline :
  public vtkStreamingDemandDrivenPipeline
{
public:
  static vtkCachedStreamingDemandDrivenPipeline* New();
  vtkTypeMacro(vtkCachedStreamingDemandDrivenPipeline,
                       vtkStreamingDemandDrivenPipeline);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * This is the maximum number of images that can be retained in memory.
   * it defaults to 10.
   */
  void SetCacheSize(int size);
  vtkGetMacro(CacheSize, int);
  //@}

protected:
  vtkCachedStreamingDemandDrivenPipeline();
  ~vtkCachedStreamingDemandDrivenPipeline() override;

  int NeedToExecuteData(int outputPort,
                                vtkInformationVector** inInfoVec,
                                vtkInformationVector* outInfoVec) override;
  int ExecuteData(vtkInformation* request,
                          vtkInformationVector** inInfoVec,
                          vtkInformationVector* outInfoVec) override;

  int CacheSize;

  vtkDataObject **Data;
  vtkMTimeType *Times;

private:
  vtkCachedStreamingDemandDrivenPipeline(const vtkCachedStreamingDemandDrivenPipeline&) = delete;
  void operator=(const vtkCachedStreamingDemandDrivenPipeline&) = delete;
};

#endif
