/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



#ifndef Q_VTK_INTERACTOR_INTERNAL_H
#define Q_VTK_INTERACTOR_INTERNAL_H

#include <QtCore/QObject>

#include <map>
class QVTKInteractor;
class QSignalMapper;
class QTimer;

// internal class, do not use
class QVTKInteractorInternal : public QObject
{
  Q_OBJECT
public:
  QVTKInteractorInternal(QVTKInteractor* p);
  ~QVTKInteractorInternal() override;
public Q_SLOTS:
  void TimerEvent(int id);
public:
  QSignalMapper* SignalMapper;
  typedef std::map<int, QTimer*> TimerMap;
  TimerMap Timers;
  QVTKInteractor* Parent;
};


#endif
