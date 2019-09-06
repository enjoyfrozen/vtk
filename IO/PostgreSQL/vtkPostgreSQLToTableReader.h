/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPostgreSQLToTableReader
 * @brief   Read a PostgreSQL table as a vtkTable
 *
 * vtkPostgreSQLToTableReader reads a table from a PostgreSQL database and
 * outputs it as a vtkTable.
*/

#ifndef vtkPostgreSQLToTableReader_h
#define vtkPostgreSQLToTableReader_h

#include "vtkIOPostgreSQLModule.h" // For export macro
#include "vtkDatabaseToTableReader.h"

class vtkPostgreSQLDatabase;

class VTKIOPOSTGRESQL_EXPORT vtkPostgreSQLToTableReader :
  public vtkDatabaseToTableReader
{
public:
  static vtkPostgreSQLToTableReader *New();
  vtkTypeMacro(vtkPostgreSQLToTableReader,vtkDatabaseToTableReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
   vtkPostgreSQLToTableReader();
  ~vtkPostgreSQLToTableReader();
  int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *) override;
private:
  vtkPostgreSQLToTableReader(const vtkPostgreSQLToTableReader&) = delete;
  void operator=(const vtkPostgreSQLToTableReader&) = delete;
};

#endif
