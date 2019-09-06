/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkQtTimePointUtility.h"

#include "vtkObjectFactory.h"


QDateTime vtkQtTimePointUtility::TimePointToQDateTime(vtkTypeUInt64 time)
{
  int julianDay = time / 86400000;
  QDate qdate = QDate::fromJulianDay(julianDay);
  int hour = static_cast<int>(time % 86400000) / 3600000;
  int minute = static_cast<int>(time % 3600000) / 60000;
  int second = static_cast<int>(time % 60000) / 1000;
  int millis = static_cast<int>(time % 1000);
  QTime qtime(hour, minute, second, millis);
  QDateTime dt(qdate, qtime);
  return dt;
}

vtkTypeUInt64 vtkQtTimePointUtility::QDateTimeToTimePoint(QDateTime time)
{
  vtkTypeUInt64 timePoint =
    QDateToTimePoint(time.date()) + QTimeToTimePoint(time.time());
  return timePoint;
}

vtkTypeUInt64 vtkQtTimePointUtility::QDateToTimePoint(QDate date)
{
  vtkTypeUInt64 timePoint =
    static_cast<vtkTypeUInt64>(date.toJulianDay())*86400000;
  return timePoint;
}

vtkTypeUInt64 vtkQtTimePointUtility::QTimeToTimePoint(QTime time)
{
  vtkTypeUInt64 timePoint =
    + time.hour()*3600000
    + time.minute()*60000
    + time.second()*1000
    + time.msec();
  return timePoint;
}
