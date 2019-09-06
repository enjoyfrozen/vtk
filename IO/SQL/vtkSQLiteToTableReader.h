/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSQLiteToTableReader
 * @brief   Read an SQLite table as a vtkTable
 *
 * vtkSQLiteToTableReader reads a table from an SQLite database and
 * outputs it as a vtkTable.
*/

#ifndef vtkSQLiteToTableReader_h
#define vtkSQLiteToTableReader_h

#include "vtkIOSQLModule.h" // For export macro
#include "vtkDatabaseToTableReader.h"

class vtkSQLiteDatabase;

class VTKIOSQL_EXPORT vtkSQLiteToTableReader :
  public vtkDatabaseToTableReader
{
public:
  static vtkSQLiteToTableReader *New();
  vtkTypeMacro(vtkSQLiteToTableReader,vtkDatabaseToTableReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
   vtkSQLiteToTableReader();
  ~vtkSQLiteToTableReader() override;
  int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *) override;
private:
  vtkSQLiteToTableReader(const vtkSQLiteToTableReader&) = delete;
  void operator=(const vtkSQLiteToTableReader&) = delete;
};

#endif
