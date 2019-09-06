/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkPointSetToMoleculeFilter_h
#define vtkPointSetToMoleculeFilter_h

#include "vtkDomainsChemistryModule.h" // For export macro
#include "vtkMoleculeAlgorithm.h"

class VTKDOMAINSCHEMISTRY_EXPORT vtkPointSetToMoleculeFilter : public vtkMoleculeAlgorithm
{
public:
  static vtkPointSetToMoleculeFilter* New();
  vtkTypeMacro(vtkPointSetToMoleculeFilter, vtkMoleculeAlgorithm);

  //@{
  /**
   * Get/Set if the filter should look for lines in input cells and convert them
   * into bonds.
   * default is ON.
   */
  vtkGetMacro(ConvertLinesIntoBonds, bool);
  vtkSetMacro(ConvertLinesIntoBonds, bool);
  vtkBooleanMacro(ConvertLinesIntoBonds, bool);
  //@}

protected:
  vtkPointSetToMoleculeFilter();
  ~vtkPointSetToMoleculeFilter() override = default;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  bool ConvertLinesIntoBonds;
private:
  vtkPointSetToMoleculeFilter(const vtkPointSetToMoleculeFilter&) = delete;
  void operator=(const vtkPointSetToMoleculeFilter&) = delete;
};

#endif
