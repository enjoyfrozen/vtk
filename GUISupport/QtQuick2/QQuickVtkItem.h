#pragma once

#include <QtQuick/QQuickItem>

#include <QtCore/QScopedPointer>

#include "vtkGUISupportQtQuick2Module.h" // for export macro

class vtkRenderWindow;

class QQuickVtkItemPrivate;
class VTKGUISUPPORTQTQUICK2_EXPORT QQuickVtkItem : public QQuickItem
{
  Q_OBJECT
public:
  explicit QQuickVtkItem(QQuickItem* parent = nullptr);
  ~QQuickVtkItem() override;

  /**
   * This function converts a QQuickItem rectangle in local coordinates into the corresponding VTK
   * viewport
   */
  void qtRect2vtkViewport(QRectF const&, double[4], QRectF* = nullptr);

  /**
   * This is the function called on the QtQuick render thread before the scenegraph state
   * is synchronized.
   *
   * The first time this is called a pipeline should be created and added to the window
   * on subsuequent calls is where the pipeline updates, camera manipulations, etc. and
   * other pre-render steps can be performed.
   *
   * \note At the time of this method execution, the GUI thread is blocked. Hence, it is safe to
   * perform state synchronization between the GUI elements and the VTK classes here.
   */
  virtual void syncVTK(vtkRenderWindow*) {}

protected:
  bool event(QEvent*) override;

protected:
  QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;

private:
  Q_DISABLE_COPY(QQuickVtkItem)
  Q_DECLARE_PRIVATE(QQuickVtkItem)
  QScopedPointer<QQuickVtkItemPrivate> d_ptr;
};
