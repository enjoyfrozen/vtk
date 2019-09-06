/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkSQLiteDatabaseInternals_h
#define vtkSQLiteDatabaseInternals_h

#include "vtk_sqlite.h"

class vtkSQLiteDatabaseInternals
{
public:
  sqlite3* SQLiteInstance;
};

#endif
// VTK-HeaderTest-Exclude: vtkSQLiteDatabaseInternals.h
