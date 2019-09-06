/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkMySQLDatabase.h"

#include <vtksys/SystemTools.hxx>

#include <string>

// Registration of MySQL dynamically with the vtkSQLDatabase factory method.
vtkSQLDatabase * MySQLCreateFunction(const char* URL)
{
  std::string urlstr(URL ? URL : "");
  std::string protocol, unused;
  vtkMySQLDatabase *db = 0;

  if (vtksys::SystemTools::ParseURLProtocol(urlstr, protocol, unused) &&
      protocol == "mysql")
  {
    db = vtkMySQLDatabase::New();
    db->ParseURL(URL);
  }

  return db;
}

static unsigned int vtkIOMySQLCount;

VTKIOMYSQL_EXPORT void vtkIOMySQL_AutoInit_Construct()
{
  if (++vtkIOMySQLCount == 1)
  {
    vtkSQLDatabase::RegisterCreateFromURLCallback(MySQLCreateFunction);
  }
}
