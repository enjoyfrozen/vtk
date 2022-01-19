/*=========================================================================

  Program:   Visualization Toolkit
  Module:    QQuickVTKInteractiveWidget.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "QQuickVTKInteractiveWidget.h"

// vtk includes
#include "vtkAbstractWidget.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkWidgetRepresentation.h"

//-------------------------------------------------------------------------------------------------
QQuickVTKInteractiveWidget::QQuickVTKInteractiveWidget(QObject* parent)
  : Superclass(parent)
{
}

//-------------------------------------------------------------------------------------------------
void QQuickVTKInteractiveWidget::setWidget(vtkAbstractWidget* w)
{
  this->m_widget.Reset(w);
}

//-------------------------------------------------------------------------------------------------
vtkAbstractWidget* QQuickVTKInteractiveWidget::widget() const
{
  auto widget = this->widgetOwned();
  // Extract the pointer. The caller doesn't know if it owns this or not, so it
  // cannot be passed back with a new reference without leaking in existing
  // code.
  vtkAbstractWidget* widget_ptr = widget;
  // XXX(thread-safety): This may not be valid after this function returns if
  // the widget is released on other threads. Previous code had problems with
  // this, so this is no worse than before.
  return widget_ptr;
}

//-------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkAbstractWidget> QQuickVTKInteractiveWidget::widgetOwned() const
{
  return this->m_widget.Lock();
}

//-------------------------------------------------------------------------------------------------
void QQuickVTKInteractiveWidget::setEnabled(bool e)
{
  if (this->m_enabled == e)
  {
    return;
  }

  this->m_enabled = e;
  Q_EMIT this->enabledChanged(this->m_enabled);
}

//-------------------------------------------------------------------------------------------------
bool QQuickVTKInteractiveWidget::enabled() const
{
  return this->m_enabled;
}

//-------------------------------------------------------------------------------------------------
void QQuickVTKInteractiveWidget::sync(vtkRenderer* ren)
{
  auto widget = this->m_widget.Lock();
  if (!ren || !widget)
  {
    return;
  }

  auto iren = ren->GetRenderWindow()->GetInteractor();
  widget->SetInteractor(iren);
  widget->SetCurrentRenderer(ren);
  widget->SetEnabled(this->m_enabled);
  widget->SetProcessEvents(this->m_enabled);
}
