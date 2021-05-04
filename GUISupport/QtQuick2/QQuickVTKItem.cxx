#include "QQuickVTKItem.h"
#include "QSGVtkOpenGLNode.h"

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGRenderNode>
#include <QtQuick/QSGRendererInterface>

#include <QtGui/QOpenGLTextureBlitter>

#include <QtCore/QMap>
#include <QtCore/QQueue>
#include <QtCore/QRunnable>
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>

#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkOpenGLFramebufferObject.h"
#include "vtkOpenGLState.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTextureObject.h"

#include "QVTKInteractor.h"
#include "QVTKInteractorAdapter.h"

#include <limits>

/* -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */

class QQuickVTKItemPrivate
{
public:
  QQuickVTKItemPrivate(QQuickVTKItem* ptr)
    : q_ptr(ptr)
  {
  }

  QQueue<QSharedPointer<QEvent>> qtEvents;
  QVTKInteractorAdapter qt2vtkInteractorAdapter;

private:
  Q_DISABLE_COPY(QQuickVTKItemPrivate)
  Q_DECLARE_PUBLIC(QQuickVTKItem)
  QQuickVTKItem* const q_ptr;
};

/* -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */

QQuickVTKItem::QQuickVTKItem(QQuickItem* parent)
  : QQuickItem(parent)
  , d_ptr(new QQuickVTKItemPrivate(this))
{
  setAcceptHoverEvents(true);
#if 0
    setAcceptTouchEvents(true);
#endif
  setAcceptedMouseButtons(Qt::AllButtons);

  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemHasContents);
}

QQuickVTKItem::~QQuickVTKItem() {}

void QQuickVTKItem::qtRect2vtkViewport(QRectF const& qtRect, double vtkViewport[4], QRectF* glRect)
{
  // Calculate our scaled size
  auto sz = size() * window()->devicePixelRatio();

  // Use a temporary if not supplied by caller
  QRectF tmp;
  if (!glRect)
    glRect = &tmp;

  // Convert origin to be bottom-left
  *glRect = QRectF{ { qtRect.x(), sz.height() - qtRect.bottom() - 1.0 }, qtRect.size() };

  // Convert to a vtkViewport
  if (vtkViewport)
  {
    vtkViewport[0] = glRect->topLeft().x() / (sz.width() - 1.0);
    vtkViewport[1] = glRect->topLeft().y() / (sz.height() - 1.0);
    vtkViewport[2] = glRect->bottomRight().x() / (sz.width() - 1.0);
    vtkViewport[3] = glRect->bottomRight().y() / (sz.height() - 1.0);
  };
}

QSGNode* QQuickVTKItem::updatePaintNode(QSGNode* node, UpdatePaintNodeData*)
{
  Q_D(QQuickVTKItem);

  auto* n = static_cast<QSGVtkOpenGLNode*>(node);

  // Don't create the node if our size is invalid
  if (!n && (width() <= 0 || height() <= 0))
    return nullptr;

  // Create the QSGRenderNode
  if (!n)
  {
    switch (auto api = window()->rendererInterface()->graphicsApi())
    {
      case QSGRendererInterface::OpenGL:
        Q_FALLTHROUGH();
      case QSGRendererInterface::OpenGLRhi:
        n = new QSGVtkOpenGLNode;
        break;
      default:
        qWarning().nospace() << "QQuickVTKItem.cpp:" << __LINE__ << ", Unsupported graphicsApi()"
                             << api;
        return nullptr;
    }
  }

  // Initializes the VTK window
  if (!n->vtkWindow)
  {
    // Create and initialize the vtkWindow
    n->vtkWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    n->vtkWindow->SetMultiSamples(0);
    n->vtkWindow->SetReadyForRendering(false);
    n->vtkWindow->SetFrameBlitModeToNoBlit();
    vtkNew<QVTKInteractor> iren;
    iren->SetRenderWindow(n->vtkWindow);
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);
    n->vtkWindow->SetReadyForRendering(false);
    iren->Initialize();
    n->vtkWindow->SetMapped(true);
    n->vtkWindow->SetIsCurrent(true);
    n->vtkWindow->SetForceMaximumHardwareLineWidth(1);
    n->vtkWindow->SetOwnContext(false);
    n->vtkWindow->OpenGLInitContext();

    initializeVTK(n->vtkWindow);
  }

  // Render VTK into pixels
  if (n->vtkWindow)
  {
    // Calculate our scaled size
    auto sz = size() * window()->devicePixelRatio();

    // Forward size changes to VTK
    n->vtkWindow->SetSize(sz.width(), sz.height());
    n->vtkWindow->GetInteractor()->SetSize(n->vtkWindow->GetSize());

    // Forward events to VTK
    while (d->qtEvents.size())
      d->qt2vtkInteractorAdapter.ProcessEvent(
        d->qtEvents.dequeue().data(), n->vtkWindow->GetInteractor());

    // Allow derived classes to update VTK state
    syncVTK(n->vtkWindow);

    // Render VTK into it's framebuffer
    auto ostate = n->vtkWindow->GetState();
    ostate->Reset();
    ostate->Push();
    ostate->vtkglDepthFunc(GL_LEQUAL); // note: By default, Qt sets the depth function to GL_LESS
                                       // but VTK expects GL_LEQUAL
    n->vtkWindow->SetReadyForRendering(true);
    n->vtkWindow->GetInteractor()->Render();
    n->vtkWindow->SetReadyForRendering(false);
    ostate->Pop();

    // Synchronize the shared state between the qt-gui-thread and the qsg-render-thread
    n->vtkTextureId =
      n->vtkWindow->GetDisplayFramebuffer()->GetColorAttachmentAsTextureObject(0)->GetHandle();
    n->qtItemSize = size();

    // Mark the node dirty so the qsg-render-thread refreshes the window pixels via render() on our
    // QSGRenderNode
    n->markDirty(QSGRenderNode::DirtyForceUpdate);
  }

  return n;
}

bool QQuickVTKItem::event(QEvent* ev)
{
  Q_D(QQuickVTKItem);

  if (!ev)
    return false;

  switch (ev->type())
  {
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
    {
      auto e = static_cast<QHoverEvent*>(ev);
      d->qtEvents.enqueue(QSharedPointer<QEvent>(
        new QHoverEvent(e->type(), e->posF(), e->oldPosF(), e->modifiers())));
      break;
    }
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
      auto e = static_cast<QKeyEvent*>(ev);
      d->qtEvents.enqueue(QSharedPointer<QEvent>(
        new QKeyEvent(e->type(), e->key(), e->modifiers(), e->nativeScanCode(),
          e->nativeVirtualKey(), e->nativeModifiers(), e->text(), e->isAutoRepeat(), e->count())));
      break;
    }
    case QEvent::FocusIn:
    case QEvent::FocusOut:
    {
      auto e = static_cast<QFocusEvent*>(ev);
      d->qtEvents.enqueue(QSharedPointer<QEvent>(new QFocusEvent(e->type(), e->reason())));
      break;
    }
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    {
      auto e = static_cast<QMouseEvent*>(ev);
      d->qtEvents.enqueue(QSharedPointer<QEvent>(new QMouseEvent(e->type(), e->localPos(),
        e->windowPos(), e->screenPos(), e->button(), e->buttons(), e->modifiers(), e->source())));
      break;
    }
#ifndef QT_NO_WHEELEVENT
    case QEvent::Wheel:
    {
      auto e = static_cast<QWheelEvent*>(ev);
      d->qtEvents.enqueue(
        QSharedPointer<QEvent>(new QWheelEvent(e->position(), e->globalPosition(), e->pixelDelta(),
          e->angleDelta(), e->buttons(), e->modifiers(), e->phase(), e->inverted(), e->source())));
      break;
    }
#endif
#if 0
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:
    {
        d->qtEvents.enqueue(QSharedPointer<QEvent>(
            new QTouchEvent(e->type(),
                e->device(),
                e->modifiers(),
                e->touchPointStates(),
                e->touchPoints())));
        break;
    }
#endif
    default:
      return QQuickItem::event(ev);
  }

  ev->accept();

  update();

  return true;
}
