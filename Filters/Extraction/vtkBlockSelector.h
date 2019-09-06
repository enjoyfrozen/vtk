/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class vtkBlockSelector
 * @brief Selects cells or points contained in a block as defined in the
 * vtkSelectionNode used to initialize this operator.
 *
 */

#ifndef vtkBlockSelector_h
#define vtkBlockSelector_h

#include "vtkSelector.h"

class VTKFILTERSEXTRACTION_EXPORT vtkBlockSelector : public vtkSelector
{
public:
  static vtkBlockSelector* New();
  vtkTypeMacro(vtkBlockSelector, vtkSelector);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void Initialize(vtkSelectionNode* node) override;

protected:
  vtkBlockSelector();
  ~vtkBlockSelector() override;

  bool ComputeSelectedElements(vtkDataObject* input, vtkSignedCharArray* insidednessArray) override;
  SelectionMode GetAMRBlockSelection(unsigned int level, unsigned int index) override;
  SelectionMode GetBlockSelection(unsigned int compositeIndex) override;

private:
  vtkBlockSelector(const vtkBlockSelector&) = delete;
  void operator=(const vtkBlockSelector&) = delete;

  class vtkInternals;
  vtkInternals* Internals;
};

#endif
