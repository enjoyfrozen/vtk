/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPExtractGrid
 * @brief   Extract VOI and/or sub-sample a distributed
 *  structured dataset.
 *
 *
 *  vtkPExtractVOI inherits from vtkExtractVOI and provides additional
 *  functionality when dealing with a distributed dataset. Specifically, when
 *  sub-sampling a dataset, a gap may be introduced between partitions. This
 *  filter handles such cases correctly by growing the grid to the right to
 *  close the gap.
 *
 * @sa
 *  vtkExtractVOI
*/

#ifndef vtkPExtractVOI_h
#define vtkPExtractVOI_h

#include "vtkFiltersParallelMPIModule.h" // For export macro
#include "vtkExtractVOI.h"

// Forward Declarations
class vtkInformation;
class vtkInformationVector;
class vtkMPIController;

class VTKFILTERSPARALLELMPI_EXPORT vtkPExtractVOI : public vtkExtractVOI
{
public:
  static vtkPExtractVOI* New();
  vtkTypeMacro(vtkPExtractVOI,vtkExtractVOI);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkPExtractVOI();
  virtual ~vtkPExtractVOI();

  // Standard VTK Pipeline methods
  virtual int RequestData(
      vtkInformation*, vtkInformationVector**,vtkInformationVector*) override;
  virtual int RequestInformation(
      vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  virtual int RequestUpdateExtent(
      vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  vtkMPIController* Controller;

private:
  vtkPExtractVOI(const vtkPExtractVOI&) = delete;
  void operator=(const vtkPExtractVOI&) = delete;
};

#endif
