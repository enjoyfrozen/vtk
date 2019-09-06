/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkImageDataToPointSet
 * @brief   Converts a vtkImageData to a vtkPointSet
 *
 *
 * vtkImageDataToPointSet takes a vtkImageData as an image and outputs an
 * equivalent vtkStructuredGrid (which is a subclass of vtkPointSet).
 *
 * @par Thanks:
 * This class was developed by Kenneth Moreland (kmorel@sandia.gov) from
 * Sandia National Laboratories.
*/

#ifndef vtkImageDataToPointSet_h
#define vtkImageDataToPointSet_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkStructuredGridAlgorithm.h"

class vtkImageData;
class vtkStructuredData;

class VTKFILTERSGENERAL_EXPORT vtkImageDataToPointSet : public vtkStructuredGridAlgorithm
{
public:
  vtkTypeMacro(vtkImageDataToPointSet, vtkStructuredGridAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  static vtkImageDataToPointSet *New();

protected:
  vtkImageDataToPointSet();
  ~vtkImageDataToPointSet() override;

  int RequestData(vtkInformation *request,
                  vtkInformationVector **inputVector,
                  vtkInformationVector *outputVector) override;

  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkImageDataToPointSet(const vtkImageDataToPointSet &) = delete;
  void operator=(const vtkImageDataToPointSet &) = delete;
};


#endif //vtkImageDataToPointSet_h
