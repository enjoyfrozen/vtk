/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkRemoveIsolatedVertices
 * @brief   remove vertices of a vtkGraph with
 *    degree zero.
 *
 *
*/

#ifndef vtkRemoveIsolatedVertices_h
#define vtkRemoveIsolatedVertices_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkGraphAlgorithm.h"

class vtkDataSet;

class VTKINFOVISCORE_EXPORT vtkRemoveIsolatedVertices : public vtkGraphAlgorithm
{
public:
  static vtkRemoveIsolatedVertices* New();
  vtkTypeMacro(vtkRemoveIsolatedVertices,vtkGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkRemoveIsolatedVertices();
  ~vtkRemoveIsolatedVertices() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

private:
  vtkRemoveIsolatedVertices(const vtkRemoveIsolatedVertices&) = delete;
  void operator=(const vtkRemoveIsolatedVertices&) = delete;
};

#endif

