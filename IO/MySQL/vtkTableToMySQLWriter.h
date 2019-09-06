/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTableToMySQLWriter
 * @brief   store a vtkTable in a MySQL database
 *
 * vtkTableToMySQLWriter reads a vtkTable and inserts it into a MySQL
 * database.
*/

#ifndef vtkTableToMySQLWriter_h
#define vtkTableToMySQLWriter_h

#include "vtkIOMySQLModule.h" // For export macro
#include "vtkTableToDatabaseWriter.h"

class vtkMySQLDatabase;

class VTKIOMYSQL_EXPORT vtkTableToMySQLWriter : public vtkTableToDatabaseWriter
{
public:
  static vtkTableToMySQLWriter *New();
  vtkTypeMacro(vtkTableToMySQLWriter,vtkTableToDatabaseWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get the input to this writer.
   */
  vtkTable* GetInput();
  vtkTable* GetInput(int port);
  //@}

protected:
   vtkTableToMySQLWriter();
  ~vtkTableToMySQLWriter();
  void WriteData() override;

  int FillInputPortInformation(int port, vtkInformation *info) override;

  vtkTable *Input;

private:
  vtkTableToMySQLWriter(const vtkTableToMySQLWriter&) = delete;
  void operator=(const vtkTableToMySQLWriter&) = delete;
};

#endif
