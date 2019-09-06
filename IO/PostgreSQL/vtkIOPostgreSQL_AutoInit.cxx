/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkPostgreSQLDatabase.h"

#include <vtksys/SystemTools.hxx>

#include <string>

// Registration of PostgreSQL dynamically with the vtkSQLDatabase factory method.
vtkSQLDatabase * PostgreSQLCreateFunction(const char* URL)
{
  std::string urlstr(URL ? URL : "");
  std::string protocol, unused;
  vtkPostgreSQLDatabase *db = 0;

  if (vtksys::SystemTools::ParseURLProtocol(urlstr, protocol, unused) &&
      protocol == "psql")
  {
    db = vtkPostgreSQLDatabase::New();
    db->ParseURL(URL);
  }

  return db;
}

static unsigned int vtkIOPostgreSQLCount;

struct VTKIOPOSTGRESQL_EXPORT vtkIOPostgreSQL_AutoInit
{
  vtkIOPostgreSQL_AutoInit();
  ~vtkIOPostgreSQL_AutoInit();
};

VTKIOPOSTGRESQL_EXPORT void vtkIOPostgreSQL_AutoInit_Construct()
{
  if (++vtkIOPostgreSQLCount == 1)
  {
    vtkSQLDatabase::RegisterCreateFromURLCallback(PostgreSQLCreateFunction);
  }
}
