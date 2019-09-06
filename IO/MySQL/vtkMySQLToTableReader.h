/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkMySQLToTableReader
 * @brief   Read a MySQL table as a vtkTable
 *
 * vtkMySQLToTableReader reads a table from a MySQL database and
 * outputs it as a vtkTable.
*/

#ifndef vtkMySQLToTableReader_h
#define vtkMySQLToTableReader_h

#include "vtkIOMySQLModule.h" // For export macro
#include "vtkDatabaseToTableReader.h"

class vtkMySQLDatabase;

class VTKIOMYSQL_EXPORT vtkMySQLToTableReader :
  public vtkDatabaseToTableReader
{
public:
  static vtkMySQLToTableReader *New();
  vtkTypeMacro(vtkMySQLToTableReader,vtkDatabaseToTableReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
   vtkMySQLToTableReader();
  ~vtkMySQLToTableReader();
  int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *) override;
private:
  vtkMySQLToTableReader(const vtkMySQLToTableReader&) = delete;
  void operator=(const vtkMySQLToTableReader&) = delete;

};

#endif
