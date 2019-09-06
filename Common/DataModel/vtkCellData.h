/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCellData
 * @brief   represent and manipulate cell attribute data
 *
 * vtkCellData is a class that is used to represent and manipulate
 * cell attribute data (e.g., scalars, vectors, normals, texture
 * coordinates, etc.) Special methods are provided to work with filter
 * objects, such as passing data through filter, copying data from one
 * cell to another, and interpolating data given cell interpolation weights.
*/

#ifndef vtkCellData_h
#define vtkCellData_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkDataSetAttributes.h"

class VTKCOMMONDATAMODEL_EXPORT vtkCellData : public vtkDataSetAttributes
{
public:
  static vtkCellData *New();

  vtkTypeMacro(vtkCellData,vtkDataSetAttributes);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkCellData() {} //make sure constructor and destructor are protected
  ~vtkCellData() override {}

private:
  vtkCellData(const vtkCellData&) = delete;
  void operator=(const vtkCellData&) = delete;
};

#endif



