#pragma once

#include <QtQuick/QSGRenderNode>

#include <QtCore/QScopedPointer>

#include <limits>

#include "vtkSmartPointer.h"

class QOpenGLTextureBlitter;

class vtkGenericOpenGLRenderWindow;

class QSGVtkOpenGLNode : public QSGRenderNode
{
public:
  ~QSGVtkOpenGLNode() override;

  // QSGRenderNode interface
  void releaseResources() override;
  StateFlags changedStates() const override;
  RenderingFlags flags() const override;
  QRectF rect() const override;
  void render(const RenderState* state) override;

  // Shared state between qsg-render-thread and qt-gui-thread set in
  // QQuickVTKItem::updatePaintNode()
  GLuint vtkTextureId = std::numeric_limits<GLuint>::max();
  QSizeF qtItemSize;

private:
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> vtkWindow;
  QScopedPointer<QOpenGLTextureBlitter> qtBlitter;
  friend class QQuickVTKItem;
};
