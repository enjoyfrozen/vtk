#include "QSGVtkOpenGLNode.h"

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTextureBlitter>

#include "vtkRendererCollection.h"
#include <vtkGenericOpenGLRenderWindow.h>

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

QSGVtkOpenGLNode::~QSGVtkOpenGLNode()
{
  releaseResources();
}

void QSGVtkOpenGLNode::releaseResources()
{
  // Cleanup the VTK window resources
  vtkWindow->GetRenderers()->InitTraversal();
  while (auto renderer = vtkWindow->GetRenderers()->GetNextItem())
    renderer->ReleaseGraphicsResources(vtkWindow);
  vtkWindow->ReleaseGraphicsResources(vtkWindow);

  // Cleanup the Qt window resources
  qtBlitter.reset();
}

QSGRenderNode::StateFlags QSGVtkOpenGLNode::changedStates() const
{
  return DepthState | StencilState | ScissorState | ColorState | BlendState | CullState |
    ViewportState | RenderTargetState;
}
QSGRenderNode::RenderingFlags QSGVtkOpenGLNode::flags() const
{
  return DepthAwareRendering | BoundedRectRendering |
    static_cast<QSGRenderNode::RenderingFlag>(inheritedOpacity() >= 1 ? OpaqueRendering : 0);
}
QRectF QSGVtkOpenGLNode::rect() const
{
  return QRectF{ { 0, 0 }, qtItemSize };
}

void QSGVtkOpenGLNode::render(const RenderState* state)
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

  // Configure OpenGL state
  gl->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  GLCHK;
  gl->glEnable(GL_BLEND);
  GLCHK;
  gl->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  GLCHK;
  gl->glBlendEquation(GL_FUNC_ADD);
  GLCHK;

  // Create a MVP matrix that uses QOpenGLTextureBlitter's geometry and OSGRenderNode's model and
  // projection matrices
  QMatrix4x4 Ndc2Item;
  Ndc2Item.scale(qtItemSize.width() / 2.0, -qtItemSize.height() / 2.0, 1.0);
  Ndc2Item.translate(1.0, -1.0, 0.0);
  auto vertexTransform = *state->projectionMatrix() * *matrix() * Ndc2Item;

  // blend VTK's fbo's colorBuffer0 pixels onto the window's backbuffer
  if (!qtBlitter)
  {
    qtBlitter.reset(new QOpenGLTextureBlitter);
    bool rc = qtBlitter->create();
    Q_ASSERT(rc);
    Q_UNUSED(rc)
  }
  qtBlitter->setOpacity(inheritedOpacity());
  qtBlitter->bind();
  GLCHK;
  qtBlitter->blit(vtkTextureId, vertexTransform, QOpenGLTextureBlitter::OriginBottomLeft);
  GLCHK;
}
