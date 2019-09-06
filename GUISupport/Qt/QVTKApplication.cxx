/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "QVTKApplication.h"

#if defined(VTK_USE_TDX) && (defined(Q_WS_X11) || defined(Q_OS_LINUX))
#include "vtkTDxQtUnixDevices.h"
#include <QWidget>
#include <X11/Xlib.h> // Needed for X types used in the public interface
#endif

// ----------------------------------------------------------------------------
QVTKApplication::QVTKApplication(int &Argc, char **Argv)
  : QApplication(Argc,Argv)
{
#if defined(VTK_USE_TDX) && (defined(Q_WS_X11) || defined(Q_OS_LINUX))
  this->Devices=new vtkTDxQtUnixDevices;
  QObject::connect(this->Devices,SIGNAL(CreateDevice(vtkTDxDevice *)),
                   this,SLOT(setDevice(vtkTDxDevice *)));
#endif
}

// ----------------------------------------------------------------------------
QVTKApplication::~QVTKApplication()
{
#if defined(VTK_USE_TDX) && (defined(Q_WS_X11) || defined(Q_OS_LINUX))
  delete this->Devices;
#endif
}

// ----------------------------------------------------------------------------
#if defined(VTK_USE_TDX) && (defined(Q_WS_X11) || defined(Q_OS_LINUX))
bool QVTKApplication::x11EventFilter(XEvent *event)
{
  // the only lines required in this method
  this->Devices->ProcessEvent(
    static_cast<vtkTDxUnixDeviceXEvent *>(event));
  return false;
}
#endif

// ----------------------------------------------------------------------------
#ifdef VTK_USE_TDX
void QVTKApplication::setDevice(vtkTDxDevice *device)
{
#ifdef Q_WS_X11 || Q_OS_LINUX
  emit CreateDevice(device);
#else
  (void)device; // to avoid warnings.
#endif
}
#endif
