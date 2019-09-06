/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSubdivideTetra
 * @brief   subdivide one tetrahedron into twelve for every tetra
 *
 * This filter subdivides tetrahedra in an unstructured grid into twelve tetrahedra.
*/

#ifndef vtkSubdivideTetra_h
#define vtkSubdivideTetra_h

#include "vtkFiltersModelingModule.h" // For export macro
#include "vtkUnstructuredGridAlgorithm.h"

class VTKFILTERSMODELING_EXPORT vtkSubdivideTetra : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkSubdivideTetra *New();
  vtkTypeMacro(vtkSubdivideTetra,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;


protected:
  vtkSubdivideTetra();
  ~vtkSubdivideTetra() override {}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:
  vtkSubdivideTetra(const vtkSubdivideTetra&) = delete;
  void operator=(const vtkSubdivideTetra&) = delete;
};

#endif


