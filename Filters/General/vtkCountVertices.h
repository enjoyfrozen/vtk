/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkCountVertices
 * @brief   Add a cell data array containing the number of
 * vertices per cell.
 *
 *
 * This filter adds a cell data array containing the number of vertices per
 * cell.
*/

#ifndef vtkCountVertices_h
#define vtkCountVertices_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPassInputTypeAlgorithm.h"

class VTKFILTERSGENERAL_EXPORT vtkCountVertices: public vtkPassInputTypeAlgorithm
{
public:
  static vtkCountVertices* New();
  vtkTypeMacro(vtkCountVertices, vtkPassInputTypeAlgorithm)
  void PrintSelf(ostream &os, vtkIndent indent) override;

  //@{
  /**
   * The name of the new output array containing the vertex counts.
   */
  vtkSetStringMacro(OutputArrayName)
  vtkGetStringMacro(OutputArrayName)
  //@}

protected:
  vtkCountVertices();
  ~vtkCountVertices() override;

  int RequestData(vtkInformation* request, vtkInformationVector **inInfoVec,
                  vtkInformationVector *outInfoVec) override;

  int FillOutputPortInformation(int port, vtkInformation* info) override;
  int FillInputPortInformation(int port, vtkInformation* info) override;

  char *OutputArrayName;

private:
  vtkCountVertices(const vtkCountVertices&) = delete;
  void operator=(const vtkCountVertices&) = delete;
};

#endif // vtkCountVertices_h
