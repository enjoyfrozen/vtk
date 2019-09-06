/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkODBCInternals
 * @brief   Simple class to hide ODBC structures
 *
 *
 *
 * There is no .cxx file to go along with this header.  Its sole
 * purpose is to let vtkODBCQuery and vtkODBCDatabase share the
 * variables that describe a single connection.
 *
 * @sa
 * vtkODBCDatabase vtkODBCQuery
 *
*/

#ifndef vtkODBCInternals_h
#define vtkODBCInternals_h

#include <sql.h>

class vtkODBCInternals {
  friend class vtkODBCDatabase;
  friend class vtkODBCQuery;

public:
  vtkODBCInternals()
    : Environment(0), Connection(0)
  {
  };

private:
  SQLHANDLE Environment;
  SQLHANDLE Connection;
};

#endif
// VTK-HeaderTest-Exclude: vtkODBCInternals.h
