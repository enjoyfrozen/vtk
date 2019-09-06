/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkDataObjectToTable
 * @brief   extract field data as a table
 *
 *
 * This filter is used to extract either the field, cell or point data of
 * any data object as a table.
*/

#ifndef vtkDataObjectToTable_h
#define vtkDataObjectToTable_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkTableAlgorithm.h"

class VTKINFOVISCORE_EXPORT vtkDataObjectToTable : public vtkTableAlgorithm
{
public:
  static vtkDataObjectToTable* New();
  vtkTypeMacro(vtkDataObjectToTable,vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  enum
  {
    FIELD_DATA = 0,
    POINT_DATA = 1,
    CELL_DATA = 2,
    VERTEX_DATA = 3,
    EDGE_DATA = 4
  };

  //@{
  /**
   * The field type to copy into the output table.
   * Should be one of FIELD_DATA, POINT_DATA, CELL_DATA, VERTEX_DATA, EDGE_DATA.
   */
  vtkGetMacro(FieldType, int);
  vtkSetClampMacro(FieldType, int, 0, 4);
  //@}

protected:
  vtkDataObjectToTable();
  ~vtkDataObjectToTable() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  int FieldType;

private:
  vtkDataObjectToTable(const vtkDataObjectToTable&) = delete;
  void operator=(const vtkDataObjectToTable&) = delete;
};

#endif

