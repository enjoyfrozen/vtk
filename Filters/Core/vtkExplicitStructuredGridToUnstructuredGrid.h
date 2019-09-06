/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkExplicitStructuredGridToUnstructuredGrid
 * @brief   Filter which converts an explicit structured grid into an unstructured grid.
*/

#ifndef vtkExplicitStructuredGridToUnstructuredGrid_h
#define vtkExplicitStructuredGridToUnstructuredGrid_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkUnstructuredGridAlgorithm.h"

class VTKFILTERSCORE_EXPORT vtkExplicitStructuredGridToUnstructuredGrid :
  public vtkUnstructuredGridAlgorithm
{
public:
  static vtkExplicitStructuredGridToUnstructuredGrid* New();
  vtkTypeMacro(vtkExplicitStructuredGridToUnstructuredGrid, vtkUnstructuredGridAlgorithm);

protected:
  vtkExplicitStructuredGridToUnstructuredGrid() = default;
  ~vtkExplicitStructuredGridToUnstructuredGrid() override = default;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*,
    vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkExplicitStructuredGridToUnstructuredGrid(
    const vtkExplicitStructuredGridToUnstructuredGrid&) = delete;
  void operator=(const vtkExplicitStructuredGridToUnstructuredGrid&) = delete;
};

#endif
