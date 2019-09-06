/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkExtractArray
 * @brief   Given a vtkArrayData object containing one-or-more
 * vtkArray instances, produces a vtkArrayData containing just one vtkArray,
 * identified by index.
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National Laboratories.
*/

#ifndef vtkExtractArray_h
#define vtkExtractArray_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkArrayDataAlgorithm.h"

class VTKFILTERSGENERAL_EXPORT vtkExtractArray : public vtkArrayDataAlgorithm
{
public:
  static vtkExtractArray* New();
  vtkTypeMacro(vtkExtractArray, vtkArrayDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Controls which array will be extracted.
   */
  vtkGetMacro(Index, vtkIdType);
  vtkSetMacro(Index, vtkIdType);
  //@}

protected:
  vtkExtractArray();
  ~vtkExtractArray() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

private:
  vtkExtractArray(const vtkExtractArray&) = delete;
  void operator=(const vtkExtractArray&) = delete;

  vtkIdType Index;
};

#endif

