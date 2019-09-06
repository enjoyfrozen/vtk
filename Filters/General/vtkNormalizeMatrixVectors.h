/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkNormalizeMatrixVectors
 * @brief   given a sparse input matrix, produces
 * a sparse output matrix with each vector normalized to unit length with respect
 * to a p-norm (default p=2).
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National Laboratories.
*/

#ifndef vtkNormalizeMatrixVectors_h
#define vtkNormalizeMatrixVectors_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkArrayDataAlgorithm.h"

class VTKFILTERSGENERAL_EXPORT vtkNormalizeMatrixVectors : public vtkArrayDataAlgorithm
{
public:
  static vtkNormalizeMatrixVectors* New();
  vtkTypeMacro(vtkNormalizeMatrixVectors, vtkArrayDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Controls whether to normalize row-vectors or column-vectors.  0 = rows, 1 = columns.
   */
  vtkGetMacro(VectorDimension, int);
  vtkSetMacro(VectorDimension, int);
  //@}

  //@{
  /**
   * Value of p in p-norm normalization, subject to p >= 1.  Default is p=2 (Euclidean norm).
   */
  vtkGetMacro(PValue, double);
  vtkSetMacro(PValue, double);
  //@}

protected:
  vtkNormalizeMatrixVectors();
  ~vtkNormalizeMatrixVectors() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  int VectorDimension;
  double PValue;

private:
  vtkNormalizeMatrixVectors(const vtkNormalizeMatrixVectors&) = delete;
  void operator=(const vtkNormalizeMatrixVectors&) = delete;
};

#endif

