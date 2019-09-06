/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class vtkGenerateGlobalIds
 * @brief generates global point and cell ids.
 *
 * vtkGenerateGlobalIds generates global point and cell ids. This filter also
 * generated ghost-point information, flagging duplicate points appropriately.
 * vtkGenerateGlobalIds works across all blocks in the input datasets and across
 * all ranks.
 */

#ifndef vtkGenerateGlobalIds_h
#define vtkGenerateGlobalIds_h

#include "vtkFiltersParallelDIY2Module.h" // for export macros
#include "vtkPassInputTypeAlgorithm.h"

class vtkMultiProcessController;

class VTKFILTERSPARALLELDIY2_EXPORT vtkGenerateGlobalIds : public vtkPassInputTypeAlgorithm
{
public:
  static vtkGenerateGlobalIds* New();
  vtkTypeMacro(vtkGenerateGlobalIds, vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get/Set the controller to use. By default
   * vtkMultiProcessController::GlobalController will be used.
   */
  void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  //@}

protected:
  vtkGenerateGlobalIds();
  ~vtkGenerateGlobalIds() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkGenerateGlobalIds(const vtkGenerateGlobalIds&) = delete;
  void operator=(const vtkGenerateGlobalIds&) = delete;

  vtkMultiProcessController* Controller;
};

#endif
