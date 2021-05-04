#pragma once

#include <QtQuick/QQuickItem>

#include <QtCore/QScopedPointer>

#include "vtkGUISupportQtQuick2Module.h" // for export macro

class vtkRenderWindow;

class QQuickVTKItemPrivate;
class VTKGUISUPPORTQTQUICK2_EXPORT QQuickVTKItem : public QQuickItem
{
  Q_OBJECT
public:
  explicit QQuickVTKItem(QQuickItem* parent = nullptr);
  ~QQuickVTKItem() override;

  /**
   * This is where the VTK initializiation should be done including creating pipeline and attaching
   * it to the window
   *
   * \note At the time of this method execution, the GUI thread is blocked. Hence, it is safe to
   * perform state synchronization between the GUI elements and the VTK classes here.
   */
  virtual void initializeVTK(vtkRenderWindow*) {}

  /**
   * This is the function called on the QtQuick render thread before the scenegraph state
   * is synchronized.
   *
   * This is where the pipeline updates, camera manipulations, etc. and other pre-render steps can
   * be performed.
   *
   * \note At the time of this method execution, the GUI thread is blocked. Hence, it is safe to
   * perform state synchronization between the GUI elements and the VTK classes here.
   */
  virtual void syncVTK(vtkRenderWindow*) {}

  /**
   * This function converts a QQuickItem rectangle in local coordinates into the corresponding VTK
   * viewport
   */
  void qtRect2vtkViewport(QRectF const&, double[4], QRectF* = nullptr);

protected:
  bool event(QEvent*) override;

protected:
  QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;

private:
  Q_DISABLE_COPY(QQuickVTKItem)
  Q_DECLARE_PRIVATE(QQuickVTKItem)
  QScopedPointer<QQuickVTKItemPrivate> d_ptr;
};
