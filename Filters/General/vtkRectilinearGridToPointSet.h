/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkRectilinearGridToPointSet
 * @brief   Converts a vtkRectilinearGrid to a vtkPointSet
 *
 *
 * vtkRectilinearGridToPointSet takes a vtkRectilinearGrid as an image and
 * outputs an equivalent vtkStructuredGrid (which is a subclass of
 * vtkPointSet).
 *
 * @par Thanks:
 * This class was developed by Kenneth Moreland (kmorel@sandia.gov) from
 * Sandia National Laboratories.
*/

#ifndef vtkRectilinearGridToPointSet_h
#define vtkRectilinearGridToPointSet_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkStructuredGridAlgorithm.h"

class vtkRectilinearGrid;
class vtkStructuredData;

class VTKFILTERSGENERAL_EXPORT vtkRectilinearGridToPointSet : public vtkStructuredGridAlgorithm
{
public:
  vtkTypeMacro(vtkRectilinearGridToPointSet, vtkStructuredGridAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  static vtkRectilinearGridToPointSet *New();

protected:
  vtkRectilinearGridToPointSet();
  ~vtkRectilinearGridToPointSet() override;

  int RequestData(vtkInformation *request,
                  vtkInformationVector **inputVector,
                  vtkInformationVector *outputVector) override;

  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkRectilinearGridToPointSet(const vtkRectilinearGridToPointSet &) = delete;
  void operator=(const vtkRectilinearGridToPointSet &) = delete;

  int CopyStructure(vtkStructuredGrid *outData, vtkRectilinearGrid *inData);
};


#endif //vtkRectilinearGridToPointSet_h
