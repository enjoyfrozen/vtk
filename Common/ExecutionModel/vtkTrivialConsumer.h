/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTrivialConsumer
 * @brief   Consumer to consume data off of a pipeline.
 *
 * vtkTrivialConsumer caps off a pipeline so that no output data is left
 * hanging around when a pipeline executes when data is set to be released (see
 * vtkDataObject::SetGlobalReleaseDataFlag). This is intended to be used for
 * tools such as Catalyst and not end users.
*/

#ifndef vtkTrivialConsumer_h
#define vtkTrivialConsumer_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkAlgorithm.h"

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkTrivialConsumer : public vtkAlgorithm
{
public:
  static vtkTrivialConsumer *New();
  vtkTypeMacro(vtkTrivialConsumer,vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkTrivialConsumer();
  ~vtkTrivialConsumer() override;

  int FillInputPortInformation(int, vtkInformation*) override;
  int FillOutputPortInformation(int, vtkInformation*) override;
private:
  vtkTrivialConsumer(const vtkTrivialConsumer&) = delete;
  void operator=(const vtkTrivialConsumer&) = delete;
};

#endif
