/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageDataToExplicitStructuredGrid
 * @brief   Filter which converts a 3D image data into an explicit structured grid.
*/

#ifndef vtkImageDataToExplicitStructuredGrid_h
#define vtkImageDataToExplicitStructuredGrid_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkExplicitStructuredGridAlgorithm.h"

class VTKFILTERSCORE_EXPORT vtkImageDataToExplicitStructuredGrid :
  public vtkExplicitStructuredGridAlgorithm
{
public:
  static vtkImageDataToExplicitStructuredGrid* New();
  vtkTypeMacro(vtkImageDataToExplicitStructuredGrid, vtkExplicitStructuredGridAlgorithm);

protected:
  vtkImageDataToExplicitStructuredGrid() = default;
  ~vtkImageDataToExplicitStructuredGrid() override = default;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*,
    vtkInformationVector**, vtkInformationVector*) override;
  int RequestInformation(vtkInformation*,
    vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkImageDataToExplicitStructuredGrid(
    const vtkImageDataToExplicitStructuredGrid&) = delete;
  void operator=(const vtkImageDataToExplicitStructuredGrid&) = delete;
};

#endif
