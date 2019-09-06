/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkRemoveDuplicatePolys
 * @brief   remove duplicate/degenerate polygons
 *
 * vtkRemoveDuplicatePolys is a filter that removes duplicate or degenerate
 * polygons. Assumes the input grid does not contain duplicate points. You
 * may want to run vtkCleanPolyData first to assert it. If duplicated
 * polygons are found they are removed in the output.
 *
 * @sa
 * vtkCleanPolyData
*/

#ifndef vtkRemoveDuplicatePolys_h
#define vtkRemoveDuplicatePolys_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSCORE_EXPORT vtkRemoveDuplicatePolys : public vtkPolyDataAlgorithm
{
public:
  static vtkRemoveDuplicatePolys *New();
  void PrintSelf(ostream& os, vtkIndent indent) override;
  vtkTypeMacro(vtkRemoveDuplicatePolys,vtkPolyDataAlgorithm);

protected:
  vtkRemoveDuplicatePolys();
  ~vtkRemoveDuplicatePolys() override;

  // Usual data generation method.
  virtual int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *) override;

private:
  vtkRemoveDuplicatePolys(const vtkRemoveDuplicatePolys&) = delete;
  void operator=(const vtkRemoveDuplicatePolys&) = delete;
};

#endif
