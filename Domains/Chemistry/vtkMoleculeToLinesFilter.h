/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkMoleculeToLinesFilter
 * @brief Convert a molecule into a simple polydata with lines.
 *
 * vtkMoleculeToLinesFilter is a filter class that takes vtkMolecule as input and
 * generates polydata on output.
 * Conversion is done following this rules:
 *  - 1 atom == 1 point
 *  - 1 bond == 1 line (cell of type VTK_LINE)
 *  - atom data is copied as point data
 *  - bond data is copied as cell data
 */

#ifndef vtkMoleculeToLinesFilter_h
#define vtkMoleculeToLinesFilter_h

#include "vtkDomainsChemistryModule.h" // For export macro
#include "vtkMoleculeToPolyDataFilter.h"

class VTKDOMAINSCHEMISTRY_EXPORT vtkMoleculeToLinesFilter : public vtkMoleculeToPolyDataFilter
{
public:
  static vtkMoleculeToLinesFilter* New();
  vtkTypeMacro(vtkMoleculeToLinesFilter, vtkMoleculeToPolyDataFilter);

protected:
  vtkMoleculeToLinesFilter() = default;
  ~vtkMoleculeToLinesFilter() override = default;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkMoleculeToLinesFilter(const vtkMoleculeToLinesFilter&) = delete;
  void operator=(const vtkMoleculeToLinesFilter&) = delete;
};

#endif
