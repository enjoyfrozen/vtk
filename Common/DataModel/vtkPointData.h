/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPointData
 * @brief   represent and manipulate point attribute data
 *
 * vtkPointData is a class that is used to represent and manipulate
 * point attribute data (e.g., scalars, vectors, normals, texture
 * coordinates, etc.) Most of the functionality is handled by
 * vtkDataSetAttributes
*/

#ifndef vtkPointData_h
#define vtkPointData_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkDataSetAttributes.h"

class VTKCOMMONDATAMODEL_EXPORT vtkPointData : public vtkDataSetAttributes
{
public:
  static vtkPointData *New();

  vtkTypeMacro(vtkPointData,vtkDataSetAttributes);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  void NullPoint(vtkIdType ptId);

protected:
  vtkPointData() {}
  ~vtkPointData() override {}

private:
  vtkPointData(const vtkPointData&) = delete;
  void operator=(const vtkPointData&) = delete;
};

#endif


