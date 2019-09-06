/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkExtractSelectedPolyDataIds
 * @brief   extract a list of cells from a polydata
 *
 * vtkExtractSelectedPolyDataIds extracts all cells in vtkSelection from a
 * vtkPolyData.
 * @sa
 * vtkSelection
*/

#ifndef vtkExtractSelectedPolyDataIds_h
#define vtkExtractSelectedPolyDataIds_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkSelection;

class VTKFILTERSEXTRACTION_EXPORT vtkExtractSelectedPolyDataIds : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkExtractSelectedPolyDataIds,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkExtractSelectedPolyDataIds *New();

protected:
  vtkExtractSelectedPolyDataIds();
  ~vtkExtractSelectedPolyDataIds() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  // Usual data generation method
  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;

private:
  vtkExtractSelectedPolyDataIds(const vtkExtractSelectedPolyDataIds&) = delete;
  void operator=(const vtkExtractSelectedPolyDataIds&) = delete;
};

#endif
