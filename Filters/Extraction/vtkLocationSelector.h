/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class vtkLocationSelector
 * @brief selects cells containing or points near chosen point locations.
 *
 * vtkLocationSelector is vtkSelector that can select elements
 * containing or near matching elements. It handles vtkSelectionNode::LOCATIONS
 */

#ifndef vtkLocationSelector_h
#define vtkLocationSelector_h

#include "vtkSelector.h"

#include <memory> // unique_ptr

class VTKFILTERSEXTRACTION_EXPORT vtkLocationSelector : public vtkSelector
{
public:
  static vtkLocationSelector* New();
  vtkTypeMacro(vtkLocationSelector, vtkSelector);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void Initialize(vtkSelectionNode* node) override;
  void Finalize() override;

protected:
  vtkLocationSelector();
  ~vtkLocationSelector() override;

  bool ComputeSelectedElements(vtkDataObject* input, vtkSignedCharArray* insidednessArray) override;

private:
  vtkLocationSelector(const vtkLocationSelector&) = delete;
  void operator=(const vtkLocationSelector&) = delete;

  class vtkInternals;
  class vtkInternalsForPoints;
  class vtkInternalsForCells;
  std::unique_ptr<vtkInternals> Internals;
};

#endif
