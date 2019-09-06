/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkBlockIdScalars
 * @brief   generates scalars from blocks.
 *
 * vtkBlockIdScalars is a filter that generates scalars using the block index
 * for each block. Note that all sub-blocks within a block get the same scalar.
 * The new scalars array is named \c BlockIdScalars.
*/

#ifndef vtkBlockIdScalars_h
#define vtkBlockIdScalars_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"

class VTKFILTERSGENERAL_EXPORT vtkBlockIdScalars : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkBlockIdScalars* New();
  vtkTypeMacro(vtkBlockIdScalars, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkBlockIdScalars();
  ~vtkBlockIdScalars() override;

  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *) override;

  vtkDataObject* ColorBlock(vtkDataObject* input, int group);

private:
  vtkBlockIdScalars(const vtkBlockIdScalars&) = delete;
  void operator=(const vtkBlockIdScalars&) = delete;

};

#endif


