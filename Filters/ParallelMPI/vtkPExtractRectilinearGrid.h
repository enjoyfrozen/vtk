/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPExtractRectilinearGrid
 * @brief   Extract VOI and/or sub-sample a distributed
 *  rectilinear grid dataset.
 *
 *
 *  vtkPExtractRectilinearGrid inherits from vtkExtractVOI & provides additional
 *  functionality when dealing with a distributed dataset. Specifically, when
 *  sub-sampling a dataset, a gap may be introduced between partitions. This
 *  filter handles such cases correctly by growing the grid to the right to
 *  close the gap.
 *
 * @sa
 *  vtkExtractRectilinearGrid
*/

#ifndef vtkPExtractRectilinearGrid_h
#define vtkPExtractRectilinearGrid_h

#include "vtkFiltersParallelMPIModule.h" // For export macro
#include "vtkExtractRectilinearGrid.h"

// Forward Declarations
class vtkInformation;
class vtkInformationVector;
class vtkMPIController;

class VTKFILTERSPARALLELMPI_EXPORT vtkPExtractRectilinearGrid :
  public vtkExtractRectilinearGrid
{
public:
  static vtkPExtractRectilinearGrid* New();
  vtkTypeMacro(vtkPExtractRectilinearGrid,vtkExtractRectilinearGrid);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkPExtractRectilinearGrid();
  virtual ~vtkPExtractRectilinearGrid();

  // Standard VTK Pipeline methods
  virtual int RequestData(
      vtkInformation*, vtkInformationVector**,vtkInformationVector*) override;
  virtual int RequestInformation(
      vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  virtual int RequestUpdateExtent(
      vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  vtkMPIController* Controller;

private:
  vtkPExtractRectilinearGrid(const vtkPExtractRectilinearGrid&) = delete;
  void operator=(const vtkPExtractRectilinearGrid&) = delete;
};

#endif /* VTKPEXTRACTRECTILINEARGRID_H_ */
