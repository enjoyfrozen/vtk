/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSpherePuzzleArrows
 * @brief   Visualize permutation of the sphere puzzle.
 *
 * vtkSpherePuzzleArrows creates
*/

#ifndef vtkSpherePuzzleArrows_h
#define vtkSpherePuzzleArrows_h

#include "vtkFiltersModelingModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkCellArray;
class vtkPoints;
class vtkSpherePuzzle;

class VTKFILTERSMODELING_EXPORT vtkSpherePuzzleArrows : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkSpherePuzzleArrows,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkSpherePuzzleArrows *New();

  //@{
  /**
   * Permutation is an array of puzzle piece ids.
   * Arrows will be generated for any id that does not contain itself.
   * Permutation[3] = 3 will produce no arrow.
   * Permutation[3] = 10 will draw an arrow from location 3 to 10.
   */
  vtkSetVectorMacro(Permutation,int,32);
  vtkGetVectorMacro(Permutation,int,32);
  void SetPermutationComponent(int comp, int val);
  void SetPermutation(vtkSpherePuzzle *puz);
  //@}

protected:
  vtkSpherePuzzleArrows();
  ~vtkSpherePuzzleArrows() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  void AppendArrow(int id1, int id2, vtkPoints *pts, vtkCellArray *polys);

  int Permutation[32];

  double Radius;

private:
  vtkSpherePuzzleArrows(const vtkSpherePuzzleArrows&) = delete;
  void operator=(const vtkSpherePuzzleArrows&) = delete;
};

#endif
