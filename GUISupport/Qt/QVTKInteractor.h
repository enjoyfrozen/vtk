/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/


#ifndef Q_VTK_INTERACTOR_H
#define Q_VTK_INTERACTOR_H

#include "vtkGUISupportQtModule.h" // For export macro
#include "QVTKWin32Header.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <QtCore/QObject>

#include "vtkTDxConfigure.h" // defines VTK_USE_TDX
#if defined(VTK_USE_TDX) && defined(Q_OS_WIN)
class vtkTDxWinDevice;
#endif
#if defined(VTK_USE_TDX) && defined(Q_OS_MAC)
class vtkTDxMacDevice;
#endif
#if defined(VTK_USE_TDX) && (defined(Q_WS_X11) || defined(Q_OS_LINUX))
class vtkTDxDevice;
class vtkTDxUnixDevice;
#endif


class QVTKInteractorInternal;

/**
 * @class QVTKInteractor
 * @brief - an interactor for QVTKOpenGLNativeWidget (and QVTKWiget).
 *
 * QVTKInteractor handles relaying Qt events to VTK.
 * @sa QVTKOpenGLNativeWidget
 */

class VTKGUISUPPORTQT_EXPORT QVTKInteractor : public vtkRenderWindowInteractor
{
public:
  static QVTKInteractor* New();
  vtkTypeMacro(QVTKInteractor,vtkRenderWindowInteractor);

  /**
   * Enum for additional event types supported.
   * These events can be picked up by command observers on the interactor.
   */
  enum vtkCustomEvents
  {
    ContextMenuEvent = vtkCommand::UserEvent + 100,
    DragEnterEvent,
    DragMoveEvent,
    DragLeaveEvent,
    DropEvent
  };

  /**
   * Overloaded terminate app, which does nothing in Qt.
   * Use qApp->exit() instead.
   */
  void TerminateApp() override;

  /**
   * Overloaded start method does nothing.
   * Use qApp->exec() instead.
   */
  void Start() override;
  void Initialize() override;

  /**
   * Start listening events on 3DConnexion device.
   */
  virtual void StartListening();

  /**
   * Stop listening events on 3DConnexion device.
   */
  virtual void StopListening();

  /**
   * timer event slot
   */
  virtual void TimerEvent(int timerId);

#if defined(VTK_USE_TDX) && (defined(Q_WS_X11) || defined(Q_OS_LINUX))
  virtual vtkTDxUnixDevice *GetDevice();
  virtual void SetDevice(vtkTDxDevice *device);
#endif

protected:
  // constructor
  QVTKInteractor();
  // destructor
  ~QVTKInteractor() override;

  // create a Qt Timer
  int InternalCreateTimer(int timerId, int timerType, unsigned long duration) override;
  // destroy a Qt Timer
  int InternalDestroyTimer(int platformTimerId) override;
#if defined(VTK_USE_TDX) && defined(Q_OS_WIN)
  vtkTDxWinDevice *Device;
#endif
#if defined(VTK_USE_TDX) && defined(Q_OS_MAC)
  vtkTDxMacDevice *Device;
#endif
#if defined(VTK_USE_TDX) && (defined(Q_WS_X11) || defined(Q_OS_LINUX))
  vtkTDxUnixDevice *Device;
#endif

private:

  QVTKInteractorInternal* Internal;

  QVTKInteractor(const QVTKInteractor&) = delete;
  void operator=(const QVTKInteractor&) = delete;

};

#endif
