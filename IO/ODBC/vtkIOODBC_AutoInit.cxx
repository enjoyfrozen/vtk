/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkODBCDatabase.h"

#include <vtksys/SystemTools.hxx>

#include <string>

// Registration of ODBC dynamically with the vtkSQLDatabase factory method.
vtkSQLDatabase * ODBCCreateFunction(const char* URL)
{
  std::string urlstr(URL ? URL : "");
  std::string protocol, unused;
  vtkODBCDatabase *db = 0;

  if (vtksys::SystemTools::ParseURLProtocol(urlstr, protocol, unused) &&
      protocol == "odbc")
  {
    db = vtkODBCDatabase::New();
    db->ParseURL(URL);
  }

  return db;
}

static unsigned int vtkIOODBCCount;

VTKIOODBC_EXPORT void vtkIOODBC_AutoInit_Construct()
{
  if (++vtkIOODBCCount == 1)
  {
    vtkSQLDatabase::RegisterCreateFromURLCallback(ODBCCreateFunction);
  }
}
