/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



// .NAME QVTKInteractorAdapter - Handle Qt events.
// .SECTION Description
// QVTKInteractor handles relaying Qt events to VTK.

#ifndef Q_VTK_INTERACTOR_ADAPTER_H
#define Q_VTK_INTERACTOR_ADAPTER_H

#include "vtkGUISupportQtModule.h" // For export macro
#include "QVTKWin32Header.h"
#include <QtCore/QObject>

class vtkRenderWindowInteractor;
class QEvent;

// .NAME QVTKInteractorAdapter - A QEvent translator.
// .SECTION Description
// QVTKInteractorAdapter translates QEvents and send them to a
// vtkRenderWindowInteractor.
class VTKGUISUPPORTQT_EXPORT QVTKInteractorAdapter : public QObject
{
  Q_OBJECT
public:
  // Description:
  // Constructor: takes QObject parent
  QVTKInteractorAdapter(QObject* parent = nullptr);

  // Description:
  // Destructor
  ~QVTKInteractorAdapter() override;

  // Description:
  // Set the device pixel ratio, this defaults to 1.0, but in Qt 5 can be != 1.0.
  void SetDevicePixelRatio(float ratio, vtkRenderWindowInteractor* iren = nullptr);
  float GetDevicePixelRatio() { return this->DevicePixelRatio; }

  // Description:
  // Process a QEvent and send it to the interactor
  // returns whether the event was recognized and processed
  bool ProcessEvent(QEvent* e, vtkRenderWindowInteractor* iren);

protected:
  int AccumulatedDelta;
  float DevicePixelRatio;
  static const double DevicePixelRatioTolerance;
};

#endif
