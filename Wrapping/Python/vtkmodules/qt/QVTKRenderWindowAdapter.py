"""Adapter class for rendering with ``vtkGenericOpenGLRenderWindow`` using a ``Qt`` ``OpenGL`` context."""

from __future__ import annotations

import qtpy
from OpenGL import GL
from packaging.version import parse
from qtpy import QtCore, QtGui, QtWidgets
from QVTKInteractorAdapter import QVTKInteractorAdapter
from vtkmodules.util.misc import calldata_type
from vtkmodules.vtkCommonCore import VTK_INT, reference, vtkCommand, vtkObject
from vtkmodules.vtkRenderingCore import (
    VTK_CURSOR_ARROW,
    VTK_CURSOR_CROSSHAIR,
    VTK_CURSOR_DEFAULT,
    VTK_CURSOR_HAND,
    VTK_CURSOR_SIZEALL,
    VTK_CURSOR_SIZENE,
    VTK_CURSOR_SIZENS,
    VTK_CURSOR_SIZENW,
    VTK_CURSOR_SIZESE,
    VTK_CURSOR_SIZESW,
    VTK_CURSOR_SIZEWE,
)
from vtkmodules.vtkRenderingOpenGL2 import vtkGenericOpenGLRenderWindow

VTK_TO_QT_CURSORS = {
    VTK_CURSOR_DEFAULT: QtCore.Qt.CursorShape.ArrowCursor,
    VTK_CURSOR_CROSSHAIR: QtCore.Qt.CursorShape.CrossCursor,
    VTK_CURSOR_SIZEALL: QtCore.Qt.CursorShape.SizeBDiagCursor,
    VTK_CURSOR_SIZENS: QtCore.Qt.CursorShape.SizeFDiagCursor,
    VTK_CURSOR_SIZEWE: QtCore.Qt.CursorShape.SizeBDiagCursor,
    VTK_CURSOR_SIZENE: QtCore.Qt.CursorShape.SizeFDiagCursor,
    VTK_CURSOR_SIZENW: QtCore.Qt.CursorShape.SizeVerCursor,
    VTK_CURSOR_SIZESE: QtCore.Qt.CursorShape.SizeHorCursor,
    VTK_CURSOR_SIZESW: QtCore.Qt.CursorShape.SizeAllCursor,
    VTK_CURSOR_HAND: QtCore.Qt.CursorShape.PointingHandCursor,
    VTK_CURSOR_ARROW: QtCore.Qt.CursorShape.ArrowCursor,
}


class QVTKRenderWindowAdapter(QtCore.QObject):
    """Helper to manage ``Qt`` context and other ``OpenGL`` components.

    ``QVTKRenderWindowAdapter`` is an internal class that is used by
    ``QVTKOpenGLNativeWidget`` and ``QVTKOpenGLWindow`` to manage rendering using a
    ``vtkGenericOpenGLRenderWindow`` within an ``OpenGL`` context created by ``Qt``.

    ``QVTKRenderWindowAdapter`` is expected to be recreated any time the OpenGL context
    changes. Upon creation, ``QVTKRenderWindowAdapter`` marks the
    ``vtkGenericOpenGLRenderWindow`` ready for rendering and initializes the ``OpenGL``
    context (``vtkOpenGLRenderWindow.OpenGLInitContext``).

    By observing events on ``vtkGenericOpenGLRenderWindow``, ``QVTKRenderWindowAdapter``
    can support rendering to an internally-created frame buffer object (FBO) via ``VTK``'s
    rendering calls. Making sure that the rendering results are shown on the screen is
    handled by ``QVTKOpenGLWindow`` or ``QVTKOpenGLNativeWidget``.
    """

    class QVTKInternals:
        def __init__(
            self,
            cntxt: QtGui.QOpenGLContext,
            renWin: vtkGenericOpenGLRenderWindow,
            widgetOrWindow: QtGui.QWindow | QtWidgets.QWidget,
            self_: QVTKRenderWindowAdapter,
        ) -> None:
            """Initialize the ``QVTKInternals`` class instance.

            Args:
                cntxt (QtGui.QOpenGLContext): ``OpenGL`` context.
                renWin (vtkGenericOpenGLRenderWindow): ``OpenGL`` render window.
                widgetOrWindow (QtGui.QWindow | QtWidget.QWidget): Widget or window.
                self_ (QVTKRenderWindowAdapter): ``QVTKRenderWindowAdapter`` instance.
            """
            assert renWin is not None
            assert cntxt is not None
            assert widgetOrWindow is not None

            self.__ParentWidget = (
                widgetOrWindow if isinstance(widgetOrWindow, QtWidgets.QWidget) else None
            )
            self.__ParentWindow = (
                widgetOrWindow if isinstance(widgetOrWindow, QtGui.QWindow) else None
            )
            self.Self = self_
            self.InteractorAdapter = QVTKInteractorAdapter(widgetOrWindow)
            self.RenderWindow = renWin
            self.Context = cntxt
            # It is unclear if we're better off creating a new ``QOpenGLContext`` with
            # shared resources or using the context passed into this method. In the end,
            # we decided to use the passed context. This way, if needed, the calling
            # code can create a new shared context and pass that to this method.
            self.Surface = self.Context.surface()

            # This flag is used to indicate that ``QVTKInteractorAdapter.render()``
            # should request the ``vtkGenericOpenGLRenderWindow`` to render. We need this
            # to avoid re-rendering when the app directly triggers a render by calling
            # ``renderWindow.Render()``.
            # Since the context is just being set up, we know that paint should request
            # VTK to do a render
            self.DoVTKRenderInPaintGL = True
            self.InPaint = False
            self.UnscaledDPI = 72  # Same default as ``vtkWindow.DPI``
            self.EnableHiDPI = True  # Default to enabling DPI scaling
            self.CustomDevicePixelRatio = 0.0

            self.format = self.Context.format()

            # renderWindowEventHandler_ref = reference(self.__renderWindowEventHandler)

            observer_commands = [
                vtkCommand.WindowMakeCurrentEvent,
                vtkCommand.WindowIsCurrentEvent,
                vtkCommand.WindowFrameEvent,
                vtkCommand.StartEvent,
                vtkCommand.EndEvent,
                vtkCommand.CursorChangedEvent,
            ]

            self.RenderWindowObserverIds = [
                self.RenderWindow.AddObserver(
                    cmd,
                    self.__renderWindowEventHandler,
                )
                for cmd in observer_commands
            ]

            # First and foremost, make sure ``vtkRenderWindow`` is not using offscreen
            # buffers as that throws off all logic to render in the buffers we're building
            # and frankly is unnecessary.
            if self.RenderWindow.GetUseOffScreenBuffers():
                self.RenderWindow.SetUseOffScreenBuffers(False)

            # Since a new context is being setup, call `OpenGLInitContext`, which does
            # things that need to be done when a new context is being created.
            self.RenderWindow.SetForceMaximumHardwareLineWidth(1)
            self.RenderWindow.SetReadyForRendering(True)
            self.RenderWindow.SetOwnContext(0)
            self.RenderWindow.OpenGLInitContext()

            self.InteractorAdapter.SetDevicePixelRatio(self.effectiveDevicePixelRatio())

        def effectiveDevicePixelRatio(self) -> float:
            if self.CustomDevicePixelRatio <= 0.0:
                return (
                    self.__ParentWindow.devicePixelRatio()
                    if self.__ParentWindow is not None
                    else self.__ParentWidget.devicePixelRatioF()
                )
            return self.CustomDevicePixelRatio

        def screenSize(self) -> QtCore.QSize:
            if self.__ParentWidget is not None:
                if parse(qtpy.QT_VERSION) >= parse('5.14.0'):
                    return self.__ParentWidget.screen().size()
                else:
                    return (
                        QtWidgets.QApplication.desktop()
                        .screenGeometry(self.__ParentWidget)
                        .size()
                    )
            elif self.__ParentWindow is not None:
                return self.__ParentWindow.screen().size()
            return QtCore.QSize()

        def makeCurrent(self) -> bool:
            assert self.Context is not None
            assert self.Surface is not None

            return self.Context.makeCurrent(self.Surface)

        def isCurrent(self) -> bool:
            assert self.Context is not None
            assert self.Surface is not None

            currentContext = QtGui.QOpenGLContext.currentContext()

            return (
                currentContext is self.Context
                and currentContext.surface() is self.Surface
            )

        def resize(
            self,
            w: int,
            h: int,
        ) -> None:
            dpr = self.effectiveDevicePixelRatio()
            deviceSize = QtCore.QSize(w, h) * dpr
            self.InteractorAdapter.SetDevicePixelRatio(dpr)

            iren = self.RenderWindow.GetInteractor()
            if iren is not None:
                iren.UpdateSize(deviceSize.width(), deviceSize.height())
            else:
                self.RenderWindow.SetSize(deviceSize.width(), deviceSize.height())

            screen_size = self.screenSize()
            screen_deviceSize = screen_size * dpr

            self.RenderWindow.SetScreenSize(
                screen_deviceSize.width(),
                screen_deviceSize.height(),
            )

            # Since we've resized, we request a vtkRenderWindow.Render in ``paintGL```
            # so that we render an updated rendering.
            self.DoVTKRenderInPaintGL = True

            # Update render window DPI, if needed, since this method gets called on
            # ``device_pixel_ratio`` changes as well.
            self.__updateDPI()

        def paint(self) -> None:
            if self.InPaint:
                return

            self.InPaint = True
            if self.DoVTKRenderInPaintGL:
                iren = self.RenderWindow.GetInteractor()
                if iren is not None:
                    iren.Render()
                else:
                    self.RenderWindow.Render()
            self.InPaint = False
            self.DoVTKRenderInPaintGL = False

        def frame(self) -> None:
            using_double_buffer = self.RenerWindow.GetDoubleBuffer()
            swap_buffers = self.RenderWindow.GetSwapBuffers()

            if using_double_buffer and not swap_buffers:
                # If we're using double buffer, but explicitly rendering to the back
                # buffer, it means we don't want the thing we've rendered displayed on the
                # screen. In this case, we ignore this frame result.
                return

            self.DoVTKRenderInPaintGL = False

            if not self.InPaint:
                if self.__ParentWidget is not None:
                    self.__ParentWindow.update()
                elif self.__ParentWindow is not None:
                    self.__ParentWindow.requestUpdate()

        def blit(
            self,
            targetId: int,
            targetAttachment: int,
            targetRect: QtCore.QRect,
            left: bool,
        ) -> bool:
            if not self.Context:
                return False

            f = self.Context.extraFunctions()

            if not f:
                return False

            f.glBindFramebuffer(GL.GL_DRAW_FRAMEBUFFER, targetId)
            bufs: list[GL.GLenum] = [targetAttachment]
            f.glDrawBuffers(1, bufs)

            scissor_test: GL.GLboolean = f.glIsEnabled(GL.GL_SCISSOR_TEST)

            if scissor_test == GL.GL_TRUE:
                self.RenderWindow.GetState().vtkglDisable(GL.GL_SCISSOR_TEST)
                f.glDisable(GL.GL_SCISSOR_TEST)  # Scissor affects glBindFrameBuffer

            srcWidth = reference(0)
            srcHeight = reference(0)

            self.RenderWindow.GetRenderFramebuffer().GetLastSize(srcWidth, srcHeight)

            self.RenderWindow.BlitDisplayFramebuffer(
                1 if left else 0,
                0,
                0,
                srcWidth,
                srcHeight,
                targetRect.x(),
                targetRect.y(),
                targetRect.width(),
                targetRect.height(),
                GL.GL_COLOR_BUFFER_BIT,
                GL.GL_LINEAR,
            )

            self.clearAlpha(targetRect)

            if scissor_test == GL.GL_TRUE:
                self.RenderWindow.GetState().vtkglEnable(GL.GL_SCISSOR_TEST)
                f.glEnable(GL.GL_SCISSOR_TEST)

            return True

        def setCursor(
            self,
            qt_or_vtk_cursor: int | QtGui.QCursor,
        ) -> None:
            if isinstance(qt_or_vtk_cursor, int):
                vtk_cursor = qt_or_vtk_cursor
                cursor = VTK_TO_QT_CURSORS.get(vtk_cursor, self.Self.defaultCursor())
                self.setCursor(cursor)
            elif isinstance(qt_or_vtk_cursor, QtGui.QCursor):
                qt_cursor = qt_or_vtk_cursor
                if self.__ParentWindow is not None:
                    self.__ParentWindow.setCursor(qt_cursor)
                else:
                    self.__ParentWidget.setCursor(qt_cursor)

        def setEnableHiDPI(
            self,
            val: bool,
        ) -> None:
            if self.EnableHiDPI != val:
                self.EnableHiDPI = val
                self.__updateDPI()

        def setUnscaledDPI(
            self,
            val: int,
        ) -> None:
            if self.UnscaledDPI != val:
                self.UnscaledDPI = val
                self.__updateDPI()

        def setCustomDevicePixelRatio(
            self,
            sf: float,
        ) -> None:
            if self.CustomDevicePixelRatio != sf:
                self.CustomDevicePixelRatio = sf
                self.__updateDPI()

        def clearAlpha(
            self,
            targetRect: QtCore.QRect,
        ) -> None:
            if qtpy.API == 'pyside6':
                # ``glGetBooleanv`` and other ``OpenGL`` functions have not been ported in
                # ``PySide6``. See https://bugreports.qt.io/browse/PYSIDE-2013
                return

            assert self.Context is not None

            f = self.Context.functions()
            if f is not None:
                # Clear alpha now. Otherwise, we end up blending the rendering with
                # background windows in certain cases.
                # This happens on MacOS when ``QSurfaceFormat.alphaBufferSize() > 0`` or
                # when using Mesa on Linux.
                # See paraview/paraview#17159
                colorMask: list[GL.GLboolean] = []
                f.glGetBooleanv(GL.GL_COLOR_WRITEMASK, colorMask)
                f.glColorMask(
                    GL.GL_FALSE,
                    GL.GL_FALSE,
                    GL.GL_FALSE,
                    GL.GL_TRUE,
                )

                clearColor: list[GL.GLfloat] = []
                f.glGetFloatv(int(GL.GL_COLOR_CLEAR_VALUE), clearColor)
                f.glClearColor(
                    0.0,
                    0.0,
                    0.0,
                    0.0,
                )

                viewport: list[GL.GLint] = []
                f.glGetIntegerv(GL.GL_VIEWPORT, viewport)
                f.glViewport(
                    targetRect.x(),
                    targetRect.y(),
                    targetRect.width(),
                    targetRect.height(),
                )

                f.glClear(GL.GL_COLOR_BUFFER_BIT)

                f.glColorMask(
                    colorMask[0],
                    colorMask[1],
                    colorMask[2],
                    colorMask[3],
                )
                f.gClearColor(
                    clearColor[0],
                    clearColor[1],
                    clearColor[2],
                    clearColor[3],
                )
                f.glViewport(
                    viewport[0],
                    viewport[1],
                    viewport[2],
                    viewport[3],
                )

        @calldata_type(VTK_INT)
        def __renderWindowEventHandler(
            self,
            __obj: vtkObject,
            eventid: int,
            callData: int | None = None,
        ) -> None:
            if eventid in {
                vtkCommand.StartEvent,
                vtkCommand.StartPickEvent,
                vtkCommand.EndEvent,
            }:
                return

            if eventid is vtkCommand.WindowMakeCurrentEvent:
                self.makeCurrent()
            elif eventid is vtkCommand.WindowIsCurrentEvent:
                cstatus = reference(callData)  # cursor status
                cstatus = self.isCurrent()
            elif eventid is vtkCommand.WindowFrameEvent:
                self.frame()
            elif eventid is vtkCommand.CursorChangedEvent:
                cShape = reference(callData)  # cursor shape
                self.setCursor(cShape)

        def __updateDPI(self) -> None:
            assert self.RenderWindow is not None

            self.RenderWindow.SetDPI(
                int(self.effectiveDevicePixelRatio() * self.UnscaledDPI)
                if self.EnableHiDPI
                else self.UnscaledDPI
            )

    def __init__(
        self,
        cntxt: QtGui.QOpenGLContext,
        renWin: vtkGenericOpenGLRenderWindow,
        widgetOrWindow: QtWidgets.QWidget | QtGui.QWindow,
    ) -> None:
        """Makes ``vtkGenericOpenGLRenderWindow`` ready for subsequent render requests.

        This calls
            - ``vtkGenericOpenGLRenderWindow.SetReadyForRendering(True),
            - ``vtkOpenGLRenderWindow.OpenGLInitContext`` to ensure the `OpenGL`` context
              is ready for ``VTK`` rendering.

        Args:
            cntxt (QtGui.QOpenGLContext): ``OpenGL`` context.
            renWin (vtkGenericOpenGLRenderWindow): ``OpenGL`` Render window.
            widgetOrWindow (QtWidgets.QWidget | QtGui.QWindow): Window or widget.
        """
        assert renWin is not None
        assert cntxt is not None
        assert widgetOrWindow is not None

        QtCore.QObject.__init__(self, widgetOrWindow)

        # In ``VTK`` C++ source, class member ``RenderWindowAdapter`` is a
        # ``QScopedPointer``, meaning it is allocated on the heap and deallocated when
        # it goes out of scope (like a smart pointer). In Python, we can achieved the
        # same effect in a context manager or manually manage object creation and
        # deletion. We'll adopt the later approach using garbage collection.
        self.Internals = self.QVTKInternals(cntxt, renWin, widgetOrWindow, self)
        self.DefaultCursor = QtCore.Qt.CursorShape.ArrowCursor

        # Need to make sure that when the context is destroyed that we release all
        # ``OpenGL`` resources
        self.Internals.Context.aboutToBeDestroyed.connect(self.contextAboutToBeDestroyed)

    @staticmethod
    def defaultFormat(
        stereo_capable: bool = False,
    ) -> QtGui.QSurfaceFormat:
        """Returns a default ``QSurfaceFormat`` for ``OpenGL`` rendering.

        If your application plans on using ``QVTKOpenGLNaiveWidget``, then this format
        (or similar) must be set as the default format on ``QSurfaceFormat`` before any
        widgets are created.

        Note this returns a ``QSurfaceFormat`` required to support the ``OpenGL``
        rendering capabilities in a ``vtkRenderWindow``. Whether those features, e.g.
        multisampling, is actually used for rendering is determined by the values
        specified on the ``vtkRenderWindow`` instance itself through the appropriate
        API.

        Passing ``stereo_capable=True`` is the same as calling
        ``QSurfacFormat.setStereo(True)``. This is necessary if you want to use
        quad-buffer stereo in your application.

        Refer to the ``Qt`` docs on ``QOpenGLWidget`` and ``QOpenGLWindow`` for the
        appropriate locations in your application where the format should be provided,
        e.g. either on the instance of the ``QOpenGLWindow``, on ``QOpenGLWidget``
        subclasses, or as a default format for the entire application using
        ``QSurfaceFormat.setDefaultFormat()``.

        Args:
            stereo_capable (bool, optional): Whether quad-buffer stereo is used.
                Defaults to False.

        Returns:
            QtGui.QSurfaceFormat: Surface format for ``VTK`` rendering with ``OpenGL``.
        """
        format = QtGui.QSurfaceFormat()

        format.setRenderableType(QtGui.QSurfaceFormat.OpenGL)
        format.setVersion(3, 2)
        format.setProfile(QtGui.QSurfaceFormat.CoreProfile)
        format.setSwapBehavior(QtGui.QSurfaceFormat.DoubleBuffer)
        format.setRedBufferSize(8)
        format.setGreenBufferSize(8)
        format.setBlueBufferSize(8)
        format.setDepthBufferSize(8)
        format.setAlphaBufferSize(8)
        format.setStencilBufferSize(0)
        format.setStereo(stereo_capable)

        # We never need multisampling in the context since the FBO can support
        # mutlisamples independently
        format.setSamples(0)

        return format

    # def context(self) -> QtGui.QOpenGLContext:
    #     """Get the ``OpenGL`` context to be used for rendering.

    #     Returns:
    #         QtGui.QOpenGLContext: The current ``OpenGL`` context.
    #     """
    #     return self.Context

    @QtCore.Slot()
    def contextAboutToBeDestroyed(self) -> None:
        self.Internals = None

    def paint(self) -> None:
        """Call this method in ``paintGL`` to request a render.

        This may trigger a ``vtkRenderWindow.Render`` if this class determines the
        buffers may be obsolete.
        """
        if self.Internals is not None:
            self.Internals.paint()

    def resize(
        self,
        width: int,
        height: int,
    ) -> None:
        """Call this method to resize the render window.

        This simply calls ``vtkRenderWindow.SetSize``, taking device pixel ratio into
        consideration. This doesn't cause a render or resize of the frame buffer object
        (FBO). That happens on a subsequent render request.

        Besdies widget resizing, this method should also be called in cases where the
        ``device_pixel_ratio`` for the parent window (or widget) changes. This is
        necessary since the internal FBO's pixel size is computed by scaling the
        ``width`` and ``height`` provided by the window's (or widget's)
        ``device_pixel_ratio``.

        Args:
            width (int): Width of the window (in pixels)
            height (int): Height of the window (in pixels)
        """
        if self.Internals is not None:
            self.Internals.resize(width, height)

    def blit(
        self,
        targetId: int,
        targetAttachment: int,
        targetRect: QtCore.QRect,
        left: bool = True,
    ) -> bool:
        """Convenience method to blit the results rendered in the internal frame buffer
        object (FBO) to a target.

        Args:
            targetId (int): Target id
            targetAttachment (int): Target attachment
            targetRect (QtCore.QRect): Target geometry
            left (bool, optional): Whether to blit for stereo left eye.
                Defaults to ``True``. (``False`` is right eye.)

        Returns:
            bool: ``True`` if successful, ``False`` otherwise.
        """
        if self.Internals is not None:
            return self.Internals.blit(
                targetId,
                targetAttachment,
                targetRect,
                left,
            )
        return False

    def blit_left_eye(
        self,
        target_id: int,
        target_attachment: int,
        target_rect: QtCore.QRect,
    ) -> bool:
        """Convenience method to blit the results rendered in the internal frame buffer
        object (FBO) to a target for the left eye when using quad buffer stereo.

        Args:
            target_id (int): Target id
            target_attachment (int): Target attachment
            target_rect (QtCore.QRect): Target geometry

        Returns:
            bool: ``True`` if successful, ``False`` otherwise.
        """
        return self.blit(target_id, target_attachment, target_rect, True)

    def blit_right_eye(
        self,
        target_id: int,
        target_attachment: int,
        target_rect: QtCore.QRect,
    ) -> bool:
        """Convenience method to blit the results rendered in the internal frame buffer
        object (FBO) to a target for the right eye when using quad buffer stereo.

        Args:
            target_id (int): Target id
            target_attachment (int): Target attachment
            target_rect (QtCore.QRect): Target geometry

        Returns:
            bool: ``True`` if successful, ``False`` otherwise.
        """
        return self.blit(target_id, target_attachment, target_rect, False)

    def handleEvent(
        self,
        evt: QtCore.QEvent,
    ) -> bool:
        """Process the ``event`` and return ``True`` if it is processed successfully.

        Args:
            event (QtCore.QEvent): Event

        Returns:
            bool: ``True`` if event processed successfully, ``False`` otherwise.
        """
        return (
            self.Internals.InteractorAdapter.ProcessEvent(
                evt, self.Internals.RenderWindow.GetInteractor()
            )
            if self.Internals is not None
            else False
        )

    def defaultCursor(self) -> QtCore.QCursor:
        return self.DefaultCursor

    def setDefaultCursor(
        self,
        cursor: QtCore.QCursor,
    ) -> None:
        self.DefaultCursor = cursor

    def setEnableHiDPI(
        self,
        value: bool,
    ) -> None:
        """Enable/disable dots per inch (DPI) scaling.

        When enabled, calls to ``resize`` (which must happen when ``device_pixel_ratio``
        or the window size changes) will result in updating the DPI on the
        ``vtkGenericOpenGLRenderWindow``. The DPI change only happens in ``resize``: this
        enables applications to temporarily change DPI on the
        ``vtkGenericOpenGLRenderWindow`` and request an explicit render seamlessly. In
        this case, it is the application's responsibility to restore DPI values.
        Otherwise, the changed value will linger until the next ``resize`` happens.

        Args:
            value (bool): Enable if ``True``, disable otherwise
        """
        if self.Internals is not None:
            self.Internals.setEnableHiDPI(value)

    def setUnscaledDPI(
        self,
        unscaledDPI: int,
    ) -> None:
        """Set the unscaled dots per inch (DPI) to use when scaling DPI.

        This defaults to 72, which is the same as the hard-coded default in ``vtkWindow``.

        Args:
            value (int): The dots per inch (DPI)
        """
        if self.Internals is not None:
            self.Internals.setUnscaledDPI(unscaledDPI)

    def setCustomDevicePixelRatio(
        self,
        sf: float,
    ) -> None:
        """Get/set the custom device pixel ratio (dpr) to use.

        If ``0`` or negative, it is treated as unset and the ``dpr`` from ``Qt`` will be
        used instead.

        Args:
            sf (float): The device pixel ratio
        """
        if self.Internals is not None:
            self.Internals.setCustomDevicePixelRatio(sf)
