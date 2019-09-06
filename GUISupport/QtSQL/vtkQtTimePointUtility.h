/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkQtTimePointUtility
 * @brief   performs common time operations
 *
 *
 * vtkQtTimePointUtility is provides methods to perform common time operations.
*/

#ifndef vtkQtTimePointUtility_h
#define vtkQtTimePointUtility_h

#include "vtkGUISupportQtSQLModule.h" // For export macro
#include "vtkObject.h"
#include <QDateTime> // Needed for method return types

class VTKGUISUPPORTQTSQL_EXPORT vtkQtTimePointUtility : public vtkObject
{
public:
  vtkTypeMacro(vtkQtTimePointUtility,vtkObject);

  static QDateTime TimePointToQDateTime(vtkTypeUInt64 time);
  static vtkTypeUInt64 QDateTimeToTimePoint(QDateTime time);
  static vtkTypeUInt64 QDateToTimePoint(QDate date);
  static vtkTypeUInt64 QTimeToTimePoint(QTime time);

protected:
  vtkQtTimePointUtility() {}
  ~vtkQtTimePointUtility() override {}

private:
  vtkQtTimePointUtility(const vtkQtTimePointUtility&) = delete;
  void operator=(const vtkQtTimePointUtility&) = delete;
};

#endif
// VTK-HeaderTest-Exclude: vtkQtTimePointUtility.h
