/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkBoostExtractLargestComponent
 * @brief   Extract the largest connected
 * component of a graph
 *
 *
 * vtkBoostExtractLargestComponent finds the largest connected region of a
 * vtkGraph. For directed graphs, this returns the largest biconnected component.
 * See vtkBoostConnectedComponents for details.
*/

#ifndef vtkBoostExtractLargestComponent_h
#define vtkBoostExtractLargestComponent_h

#include "vtkInfovisBoostGraphAlgorithmsModule.h" // For export macro
#include "vtkGraphAlgorithm.h"

class vtkGraph;

class VTKINFOVISBOOSTGRAPHALGORITHMS_EXPORT vtkBoostExtractLargestComponent : public vtkGraphAlgorithm
{
public:
  vtkTypeMacro(vtkBoostExtractLargestComponent, vtkGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Construct an instance of vtkBoostExtractLargestComponent with
   * InvertSelection set to false.
   */
  static vtkBoostExtractLargestComponent* New();

  //@{
  /**
   * Set the flag to determine if the selection should be inverted.
   */
  vtkSetMacro(InvertSelection, bool);
  vtkGetMacro(InvertSelection, bool);
  //@}

protected:
  vtkBoostExtractLargestComponent();
  ~vtkBoostExtractLargestComponent(){}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  /**
   * Store the choice of whether or not to invert the selection.
   */
  bool InvertSelection;

private:
  vtkBoostExtractLargestComponent(const vtkBoostExtractLargestComponent&) = delete;
  void operator=(const vtkBoostExtractLargestComponent&) = delete;
};

#endif
