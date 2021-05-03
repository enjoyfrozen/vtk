#include "QQuickVtkItem.h"

#include <QtQuick/QQuickWindow>
#include <QtQuick/QSGRenderNode>
#include <QtQuick/QSGRendererInterface>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
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
#include "vtkRendererCollection.h"
#include "vtkTextureObject.h"

#include "QVTKInteractor.h"
#include "QVTKInteractorAdapter.h"

#include <limits>

/* -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */

#ifndef NDEBUG
#ifdef _MSC_VER
#define __builtin_trap __debugbreak
#endif
#define GLCHK                                                                                      \
  {                                                                                                \
    GLenum err = gl->glGetError();                                                                 \
    if (GL_NO_ERROR != err)                                                                        \
      __builtin_trap();                                                                            \
  }
#else
#define __builtin_trap()                                                                           \
  {                                                                                                \
  }
#define GLCHK
#endif

/* -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */

namespace
{
struct QSGVtkOpenGLNode : QSGRenderNode
{
  ~QSGVtkOpenGLNode() override { releaseResources(); }

  void releaseResources() override
  {
    // Cleanup the VTK window resources
    vtkWindow->GetRenderers()->InitTraversal();
    while (auto renderer = vtkWindow->GetRenderers()->GetNextItem())
      renderer->ReleaseGraphicsResources(vtkWindow);
    vtkWindow->ReleaseGraphicsResources(vtkWindow);

    // Cleanup the Qt window resources
    qtBlitter.reset();
  }

  StateFlags changedStates() const override
  {
    return DepthState | StencilState | ScissorState | ColorState | BlendState | CullState |
      ViewportState | RenderTargetState;
  }
  RenderingFlags flags() const override
  {
    return DepthAwareRendering | BoundedRectRendering |
      (inheritedOpacity() >= 1 ? OpaqueRendering : 0);
  }
  QRectF rect() const { return QRectF{ { 0, 0 }, qtItemSize }; }

  void render(const RenderState* state)
  {
    auto gl = QOpenGLContext::currentContext()->functions();

    // Clip support.
    if (state->scissorEnabled())
    {
      const QRect r = state->scissorRect(); // already bottom-up
      gl->glEnable(GL_SCISSOR_TEST);
      GLCHK;
      gl->glScissor(r.x(), r.y(), r.width(), r.height());
      GLCHK;
    }
    if (state->stencilEnabled())
    {
      gl->glEnable(GL_STENCIL_TEST);
      GLCHK;
      gl->glStencilFunc(GL_EQUAL, state->stencilValue(), 0xFF);
      gl->glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
      GLCHK;
    }
    if (flags() && DepthAwareRendering)
      glEnable(GL_DEPTH_TEST);
    GLCHK;

    // blend VTK's fbo's colorBuffer0 pixels onto the window's backbuffer
    QMatrix4x4 Ndc2Item;
    Ndc2Item.scale(qtItemSize.width() / 2.0, -qtItemSize.height() / 2.0, 1.0);
    Ndc2Item.translate(1.0, -1.0, 0.0);
    auto vertexTransform = *state->projectionMatrix() * *matrix() * Ndc2Item;
    gl->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    GLCHK;
    gl->glEnable(GL_BLEND);
    GLCHK;
    gl->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    GLCHK;
    gl->glBlendEquation(GL_FUNC_ADD);
    GLCHK;
    if (!qtBlitter)
    {
      qtBlitter.reset(new QOpenGLTextureBlitter);
      qtBlitter->create();
      GLCHK;
    }
    qtBlitter->setOpacity(inheritedOpacity());
    qtBlitter->bind();
    GLCHK;
    qtBlitter->blit(vtkTextureId, vertexTransform, QOpenGLTextureBlitter::OriginBottomLeft);
    GLCHK;
  }

  // Shared state between qsg-render-thread and qt-gui-thread set in
  // QQuickVtkItem::updatePaintNode()
  GLuint vtkTextureId = std::numeric_limits<GLuint>::max();
  QSizeF qtItemSize;

private:
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> vtkWindow;
  QScopedPointer<QOpenGLTextureBlitter> qtBlitter;
  friend class QQuickVtkItem;
};
}

/* -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */

class QQuickVtkItemPrivate
{
public:
  QQuickVtkItemPrivate(QQuickVtkItem* ptr)
    : q_ptr(ptr)
  {
  }

  QQueue<QSharedPointer<QEvent>> qtEvents;
  QVTKInteractorAdapter qt2vtkInteractorAdapter;

private:
  Q_DISABLE_COPY(QQuickVtkItemPrivate)
  Q_DECLARE_PUBLIC(QQuickVtkItem)
  QQuickVtkItem* const q_ptr;
};

/* -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
 */

QQuickVtkItem::QQuickVtkItem(QQuickItem* parent)
  : QQuickItem(parent)
  , d_ptr(new QQuickVtkItemPrivate(this))
{
  setAcceptHoverEvents(true);
#if 0
    setAcceptTouchEvents(true);
#endif
  setAcceptedMouseButtons(Qt::AllButtons);

  setFlag(QQuickItem::ItemIsFocusScope);
  setFlag(QQuickItem::ItemHasContents);
}

QQuickVtkItem::~QQuickVtkItem() {}

void QQuickVtkItem::qtRect2vtkViewport(QRectF const& qtRect, double vtkViewport[4], QRectF* glRect)
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

QSGNode* QQuickVtkItem::updatePaintNode(QSGNode* node, UpdatePaintNodeData*)
{
  Q_D(QQuickVtkItem);

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
    {
      if (d->qtEvents.first()->type() == QEvent::MouseMove)
        qDebug() << static_cast<QMouseEvent*>(d->qtEvents.first().data())->pos();
      d->qt2vtkInteractorAdapter.ProcessEvent(
        d->qtEvents.dequeue().data(), n->vtkWindow->GetInteractor());
    }

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

bool QQuickVtkItem::event(QEvent* ev)
{
  Q_D(QQuickVtkItem);

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
