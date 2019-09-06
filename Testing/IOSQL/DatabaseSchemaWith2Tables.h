/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkTestingIOSQLModule.h"

class vtkSQLDatabaseSchema;

class VTKTESTINGIOSQL_EXPORT DatabaseSchemaWith2Tables
{
  public:
    DatabaseSchemaWith2Tables();
    ~DatabaseSchemaWith2Tables();
    vtkSQLDatabaseSchema* GetSchema() { return Schema; };
    int GetTableAHandle() { return TableAHandle; };
    int GetTableBHandle() { return TableBHandle; };
    vtkSQLDatabaseSchema* operator->() const { return this->Schema; };

  private:
    void Create();
    vtkSQLDatabaseSchema* Schema;
    int TableAHandle;
    int TableBHandle;
};

// VTK-HeaderTest-Exclude: DatabaseSchemaWith2Tables.h
