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
 *  vtkPExtractGrid inherits from vtkExtractGrid and provides additional
 *  functionality when dealing with a distributed dataset. Specifically, when
 *  sub-sampling a dataset, a gap may be introduced between partitions. This
 *  filter handles such cases correctly by growing the grid to the right to
 *  close the gap.
 *
 * @sa
 *  vtkExtractGrid
*/

#ifndef vtkPExtractGrid_h
#define vtkPExtractGrid_h

#include "vtkFiltersParallelMPIModule.h" // For export macro
#include "vtkExtractGrid.h"

// Forward declarations
class vtkMPIController;

class VTKFILTERSPARALLELMPI_EXPORT vtkPExtractGrid: public vtkExtractGrid
{
public:
    static vtkPExtractGrid* New();
    vtkTypeMacro(vtkPExtractGrid,vtkExtractGrid);
    void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
    vtkPExtractGrid();
    virtual ~vtkPExtractGrid();

    // Standard VTK Pipeline methods
    virtual int RequestData(
        vtkInformation*, vtkInformationVector**,vtkInformationVector*) override;
    virtual int RequestInformation(
        vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
    virtual int RequestUpdateExtent(
        vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

    vtkMPIController* Controller;

private:
    vtkPExtractGrid(const vtkPExtractGrid&) = delete;
    void operator=(const vtkPExtractGrid&) = delete;
};

#endif
