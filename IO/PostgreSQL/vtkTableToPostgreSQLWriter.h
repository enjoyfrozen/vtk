/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTableToPostgreSQLWriter
 * @brief   store a vtkTable in a PostgreSQL database
 *
 * vtkTableToPostgreSQLWriter reads a vtkTable and inserts it into a PostgreSQL
 * database.
*/

#ifndef vtkTableToPostgreSQLWriter_h
#define vtkTableToPostgreSQLWriter_h

#include "vtkIOPostgreSQLModule.h" // For export macro
#include "vtkTableToDatabaseWriter.h"

class vtkPostgreSQLDatabase;

class VTKIOPOSTGRESQL_EXPORT vtkTableToPostgreSQLWriter : public vtkTableToDatabaseWriter
{
public:
  static vtkTableToPostgreSQLWriter *New();
  vtkTypeMacro(vtkTableToPostgreSQLWriter,vtkTableToDatabaseWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get the input to this writer.
   */
  vtkTable* GetInput();
  vtkTable* GetInput(int port);
  //@}

protected:
   vtkTableToPostgreSQLWriter();
  ~vtkTableToPostgreSQLWriter();
  void WriteData() override;

  int FillInputPortInformation(int port, vtkInformation *info) override;

  vtkTable *Input;

private:
  vtkTableToPostgreSQLWriter(const vtkTableToPostgreSQLWriter&) = delete;
  void operator=(const vtkTableToPostgreSQLWriter&) = delete;
};

#endif
