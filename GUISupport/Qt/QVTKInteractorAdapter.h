// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-FileCopyrightText: Copyright 2004 Sandia Corporation
// SPDX-License-Identifier: LicenseRef-BSD-3-Clause-Sandia-USGov

/*========================================================================
 For general information about using VTK and Qt, see:
 http://www.trolltech.com/products/3rdparty/vtksupport.html
=========================================================================*/

// .NAME QVTKInteractorAdapter - Handle Qt events.
// .SECTION Description
// QVTKInteractor handles relaying Qt events to VTK.

#ifndef Q_VTK_INTERACTOR_ADAPTER_H
#define Q_VTK_INTERACTOR_ADAPTER_H

#include "QVTKWin32Header.h"
#include "vtkGUISupportQtModule.h" // For export macro
#include <QtCore/QObject>

class QEvent;

VTK_ABI_NAMESPACE_BEGIN
class vtkRenderWindowInteractor;

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

  ///@{
  /**
   * Enable/disable conversion of touch events to mouse events.
   * Conversion is not necessary for multi-touch gesture support, because Qt recognizes touch
   * gestures and this class creates gesture events (PinchEvent, RotateEvent, PanEvent, ...)
   * from them. Enabling conversion may have negative side effects, because it makes it difficult
   * to separate real mouse events from those events that are simulated from touch events.
   * This flag exists to allow access to low-level touch events in VTK (e.g., for implementing
   * custom gesture recognition in the interactor).
   * Disabled by default.
   */
  void setConvertTouchEventsToMouseEvents(bool convertTouchEventsToMouseEvents);
  bool convertTouchEventsToMouseEvents() const;
  ///@}

protected:
  int AccumulatedDelta;
  float DevicePixelRatio;
  static const double DevicePixelRatioTolerance;
  bool ConvertTouchEventsToMouseEvents;
};

VTK_ABI_NAMESPACE_END
#endif
// VTK-HeaderTest-Exclude: QVTKInteractorAdapter.h
