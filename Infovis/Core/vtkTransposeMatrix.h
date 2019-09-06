/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkTransposeMatrix
 * @brief   Computes the transpose of an input matrix.
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National Laboratories.
*/

#ifndef vtkTransposeMatrix_h
#define vtkTransposeMatrix_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkArrayDataAlgorithm.h"

class VTKINFOVISCORE_EXPORT vtkTransposeMatrix : public vtkArrayDataAlgorithm
{
public:
  static vtkTransposeMatrix* New();
  vtkTypeMacro(vtkTransposeMatrix, vtkArrayDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkTransposeMatrix();
  ~vtkTransposeMatrix() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

private:
  vtkTransposeMatrix(const vtkTransposeMatrix&) = delete;
  void operator=(const vtkTransposeMatrix&) = delete;
};

#endif

