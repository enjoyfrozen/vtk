/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkMoleculeToBondStickFilter
 * @brief   Generate polydata with cylinders
 * representing bonds
*/

#ifndef vtkMoleculeToBondStickFilter_h
#define vtkMoleculeToBondStickFilter_h

#include "vtkDomainsChemistryModule.h" // For export macro
#include "vtkMoleculeToPolyDataFilter.h"

class vtkMolecule;

class VTKDOMAINSCHEMISTRY_EXPORT vtkMoleculeToBondStickFilter
: public vtkMoleculeToPolyDataFilter
{
 public:
  vtkTypeMacro(vtkMoleculeToBondStickFilter,vtkMoleculeToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkMoleculeToBondStickFilter *New();

protected:
  vtkMoleculeToBondStickFilter();
  ~vtkMoleculeToBondStickFilter() override;

  int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *) override;

private:
  vtkMoleculeToBondStickFilter(const vtkMoleculeToBondStickFilter&) = delete;
  void operator=(const vtkMoleculeToBondStickFilter&) = delete;
};

#endif
