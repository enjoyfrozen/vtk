/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkExtractSelectedLocations
 * @brief   extract cells within a dataset that
 * contain the locations listen in the vtkSelection.
 *
 * vtkExtractSelectedLocations extracts all cells whose volume contain at least
 * one point listed in the LOCATIONS content of the vtkSelection. This filter
 * adds a scalar array called vtkOriginalCellIds that says what input cell
 * produced each output cell. This is an example of a Pedigree ID which helps
 * to trace back results.
 * @sa
 * vtkSelection vtkExtractSelection
*/

#ifndef vtkExtractSelectedLocations_h
#define vtkExtractSelectedLocations_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkExtractSelectionBase.h"

class vtkSelection;
class vtkSelectionNode;

class VTKFILTERSEXTRACTION_EXPORT vtkExtractSelectedLocations : public vtkExtractSelectionBase
{
public:
  static vtkExtractSelectedLocations *New();
  vtkTypeMacro(vtkExtractSelectedLocations, vtkExtractSelectionBase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkExtractSelectedLocations();
  ~vtkExtractSelectedLocations() override;

  // Usual data generation method
  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;

  int ExtractCells(vtkSelectionNode *sel, vtkDataSet *input,
                   vtkDataSet *output);
  int ExtractPoints(vtkSelectionNode *sel, vtkDataSet *input,
                    vtkDataSet *output);

private:
  vtkExtractSelectedLocations(const vtkExtractSelectedLocations&) = delete;
  void operator=(const vtkExtractSelectedLocations&) = delete;
};

#endif
