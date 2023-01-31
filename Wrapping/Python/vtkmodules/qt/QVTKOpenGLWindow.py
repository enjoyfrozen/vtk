"""Display a ``vtkGenericOpenGLRenderWindow`` in a ``QOpenGLWindow``.

``QVTKOpenGLWindow`` is one of those mechanisms for displaying ``VTK`` rendering
results in a ``Qt`` application. ``QVTKOpenGLWindow`` extends ``QOpenGLWindow`` to
display the rendering results of a ``vtkGenericOpenGLRendeWindow``.

Since ``QVTKOpenGLWindow`` is based on ``QOpenGLWindow``, it is intended for rendering in
a top-level window. ``QVTKOpenGLWindow`` can be embedded in another ``QWidget`` using
``QWidget.createWindowContainer`` or by using ``QVTKOpenGLStereoWidget`` instead.
However, developers are encouraged to check ``Qt`` documentation for
``QWidget.createWindowContainer`` idiosyncrasies. Using ``QVTKOpenGLNativeWidget`` instead
is generally a better choice for cases where you want to embed ``VTK`` rendering results
in a ``QWidget``. ``QVTKOpenGLWindow`` or ``QVTKOpenGLStereoWidget`` is still preferrred
for applications that want to support quad-buffer based stereo rendering.

To request a specific configuration for the ``OpenGL`` context, use
``QWindow.setFormat()`` like for any other ``QWindow``. This gives you the
ability to, among other things, request a given ``OpenGL`` version and profile. Use
``QOpenGLWindow.defaultFormat()`` to obtain a ``QSurfaceFormat`` with the appropriate
``OpenGL`` version configuration. To enable quad-buffer stereo, you'll need to call
``QSurfaceFormat.setStereo(True)``.

``VTK`` rendering features like multisampling, double buffering, etc., are enabled or
disabled by directly setting the corresponding attributes on
``vtkGenericOpenGLRenderWindow`` and not when specifying the ``OpenGL`` context format in
``setFormat``. If not specified, ``QSurfaceFormat.defaultFormat`` will be used.

Note:
    ``QVTKOpenGLWindow`` requires ``Qt`` versions ``5.9`` and above.

See Also:
    - ``QVTKOpenGLStereoWidget``
    - ``QVTKOpenGLNativeWidget``
"""

from __future__ import annotations

import functools

from OpenGL import GL
from qtpy import QtCore, QtGui
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import VTK_STEREO_CRYSTAL_EYES, vtkRenderWindow
from vtkmodules.vtkRenderingOpenGL2 import vtkGenericOpenGLRenderWindow

from .QVTKInteractor import QVTKInteractor
from .QVTKRenderWindowAdapter import QVTKRenderWindowAdapter


class QVTKOpenGLWindow(QtGui.QOpenGLWindow):

    windowEvent = QtCore.Signal(QtCore.QEvent)

    def __init__(
        self,
        renderWindow: vtkGenericOpenGLRenderWindow | None = None,
        shareContext: QtGui.QOpenGLContext | None = None,
        parent: QtGui.QWindow | None = None,
        updateBehavior: QtGui.QOpenGLWindow.UpdateBehavior = QtGui.QOpenGLWindow.NoPartialUpdate,
    ) -> None:
        QtGui.QOpenGLWindow.__init__(
            self,
            shareContext,
            updateBehavior=updateBehavior,
            parent=parent,
        )

        self.RenderWindow = (
            vtkGenericOpenGLRenderWindow() if renderWindow is None else renderWindow
        )

        self.RenderWindowAdapter = None
        self.EnableHiDPI = True
        self.UnscaledDPI = 72
        self.CustomDevicePixelRatio = 0.0
        self.DefaultCursor = QtCore.Qt.CursorShape.ArrowCursor

        self.setRenderWindow(self.RenderWindow)

    def setRenderWindow(
        self,
        win: vtkRenderWindow | vtkGenericOpenGLRenderWindow,
    ) -> None:
        """Set a render window to use.

        If a render window was already set, it will be finalized and all of its
        ``OpenGL`` resources will be released. If ``win`` is not ``None`` and it has no
        interactor set, a ``QVTKInteractor`` instance will be created and set on the
        render window as the interactor.

        Args:
            win (vtkRenderWindow | vtkGenericOpenGLRenderWindow): Render window.
        """
        if win is self.RenderWindow:
            return

        # This will release all OpenGL resources associated with the old render window,
        # if any.
        if self.RenderWindowAdapter is not None:
            self.makCurrent()
            self.RenderWindowAdapter.context().aboutToBeDestroyed.emit()
            self.RenderWindowAdapter = None

        self.RenderWindow = win
        if self.RenderWindow is not None:
            self.RenderWindow.SetReadyForRendering(False)
            self.RenderWindow.SetFrameBlitModeToNoBlit()

            # If an interactor wasn't provided, we'll make one by default
            if self.RenderWindow.GetInteractor() is not None:
                iren = QVTKInteractor()

                self.RenderWindow.SetInteractor(iren)
                iren.Initialize()

                # Now set the default style
                style = vtkInteractorStyleTrackballCamera()
                iren.SetInteractorStyle(style)

            if self.isValid():
                # This typically means that the render window is being changed after
                # the ``QVTKOpenGLWindow`` has initialized itself in a previous
                # update pass, so we emulate the steps to ensure that the new
                # ``vtkRenderWindows`` is brought to the same state (minus the
                # actual render).
                self.makeCurrent()
                self.initializeGL()
                self.updateSize()

    def renderWindow(self) -> vtkRenderWindow:
        """Return the render window that is being shown in this widget.

        Returns:
            vtkRenderWindow: The render window.
        """
        return self.RenderWindow

    def enableHiDPI(self) -> bool:
        return self.EnableHiDPI

    def setEnableHiDPI(
        self,
        enable: bool,
    ) -> None:
        """Enable/disable support for HiDPI displays.

        When enabled, this calls ``vtkWindow.SetDPI`` with a DPI value scaled by the
        device pixel ratio every time the widget is resized. The unscaled DPI value can
        be specified by using ``setUnscaledDPI``.

        Args:
            enable (bool): Enable DPI scaling if ``True``, disable otherwise.
        """
        self.EnableHiDPI = enable
        if self.RenderWindowAdapter is not None:
            self.RenderWindowAdapter.setEnableHiDPI(enable)

    def unscaledDPI(self) -> int:
        return self.UnscaledDPI

    def setUnscaledDPI(self, dpi: int) -> None:
        """Set/get the unscaled DPI value.

        Defaults to 72, which is also the default value in ``vtkWindow``.

        Args:
            dpi (int): DPI value
        """
        self.UnscaledDPI = dpi
        if self.RenderWindowAdapter is not None:
            self.RenderWindowAdapter.setUnscaledDPI(dpi)

    def setCustomDevicePixelRatio(
        self,
        cdpr: float,
    ) -> None:
        """Set/get a custom device pixel ratio to use to map ``Qt`` sizes to ``VTK`` (or
        ``OpenGL``) sizes. When the ``QWidget`` is resized, it calls
        ``vtkRenderWindow.SetSize`` on the internal ``vtkRenderWindow`` after multiplying
        the ``QWidget``'s size by this scale factor.

        By default, this is set to ``0``, which means the ``devicePixelRatio`` obtained
        from ``Qt`` will be used. Set this to a number greater than ``0`` to override this
        behavior and use the custom scale factor instead.

        ``effectiveDevicePixelRatio`` can be used to obtain the device pixel ratio that
        will be used, given the value for ``cutsomDevicePixelRatio``.

        Args:
            cdpr (float): Custom device pixel ratio
        """
        self.CustomDevicePixelRatio = cdpr
        if self.RenderWindowAdapter is not None:
            self.RenderWindowAdapter.setCustomDevicePixelRatio(cdpr)

    def customDevicePixelRatio(self) -> float:
        return self.customDevicePixelRatio

    def effectiveDevicePixelRatio(self) -> float:
        return (
            self.CustomDevicePixelRatio
            if self.CustomDevicePixelRatio > 0.0
            else self.devicePixelRatioF()
        )

    def setDefaultCursor(
        self,
        cursor: QtCore.QCursor,
    ) -> None:
        """Set/get the default cursor to use for this widget.

        Args:
            cursor (QtCore.QCursor): Cursor to use for this widget.
        """
        self.DefaultCursor = cursor
        if self.RenderWindowAdapter is not None:
            self.RenderWindowAdapter.setDefaultCursor(cursor)

    def defaultCursor(self) -> QtCore.QCursor:
        return self.DefaultCursor

    def interactor(self) -> QVTKInteractor:
        """Get the interactor that was either created by default or set by the user.

        Returns:
            QVTKInteractor: The interactor.
        """
        return (
            QVTKInteractor.SafeDownCast(self.RenderWindow.GetInteractor())
            if self.RenderWindow is not None
            else None
        )

    @QtCore.Slot()
    def updateSize(self) -> None:
        if self.RenderWindowAdapter is not None:
            self.RenderWindowAdapter.resize(self.width(), self.height())

    def event(
        self,
        evt: QtCore.QEvent,
    ) -> bool:
        # Forward event to the Widget containing this window. This is required due to
        # QTBUG-61836 that prevents the use of the flag ``Qt.TransparentForMouseInput``.
        # This flag should indicate that this window should not catch any event and let
        # them pass through to the widget.
        # The containing widget should then forward back only the required events for this
        # window (such as mouse and resize events). Until this misbehavior is fixed, we
        # have to handle forwarding of events
        self.windowEvent.emit(evt)

        if self.RenderWindowAdapter is not None:
            self.RenderWindowAdapter.handleEvent(evt)

        return super().event(evt)

    def initializeGL(self) -> None:
        super().initializeGL()
        if self.RenderWindow is not None:
            assert self.RenderWindowAdapter is None

            ostate = self.RenderWindow.GetState()
            ostate.Reset()
            # By default, ``Qt`` sets the depth function to ``GL.GL_LESS``, but ``VTK``
            # expects ``GL.GL_LEQUAL``
            ostate.vtkglDepthFunc(GL.GL_LEQUAL)

            self.RenderWindowAdapter = QVTKRenderWindowAdapter(
                self.context(),
                self.RenderWindow,
                self,
            )
            self.RenderWindowAdapter.setDefaultCursor(self.defaultCursor())
            self.RenderWindowAdapter.setEnableHiDPI(self.EnableHiDPI)
            self.RenderWindowAdapter.setUnscaledDPI(self.UnscaledDPI)
            self.RenderWindowAdapter.setCustomDevicePixelRatio(
                self.CustomDevicePixelRatio
            )

    def paintGL(self) -> None:
        super().paintGL()
        if self.RenderWindow is not None:
            ostate = self.RenderWindow.GetState()
            ostate.Reset()
            ostate.Push()
            # By default, ``Qt`` sets the depth function to ``GL_LESS``, but ``VTK``
            # expects ``GL_LEQUAL``.
            ostate.vtkglDepthFunc(GL.GL_LEQUAL)
            assert self.RenderWindowAdapter is not None
            self.RenderWindowAdapter.paint()

            # If rendering was triggered by the above calls, that may change the current
            # context due to things like progress events triggering updates on other
            # widgets (e.g. progress bar). Hence, we need to make sure to call
            # ``makeCurrent()`` before proceeding with blit-ing
            self.makeCurrent()

            deviceSize = self.size() * self.devicePixelRatioF()
            fmt = self.context().format()
            if (
                fmt.stereo()
                and self.RenderWindow.GetStereoRender()
                and self.RenderWindow.GetStereoType == VTK_STEREO_CRYSTAL_EYES
            ):
                self.RenderWindowAdapter.blitLeftEye(
                    self.defaultFramebufferObject(),
                    GL.GL_BACK_LEFT,
                    QtCore.QRect(QtCore.QPoint(0, 0)),
                    deviceSize,
                )
                self.RenderWindowAdapter.blitRightEye(
                    self.defaultFramebufferObject(),
                    GL.GL_BACK_RIGHT,
                    QtCore.QRect(QtCore.QPoint(0, 0)),
                    deviceSize,
                )
            else:
                # No render window; just fill with white.
                f = QtGui.QOpenGLContext.currentContext().functions()
                f.glClearColor(1.0, 1.0, 1.0, 1.0)
                f.glClear(GL.GL_COLOR_BUFFER_BIT)

    def resizeGL(
        self,
        w: int,
        h: int,
    ) -> None:
        """Resize the ``OpenGL`` window.

        Args:
            w (int): Width (pixels)
            h (int): Height (pixels)
        """
        super().resizeGL(w, h)
        self.updateSize()

    @staticmethod
    @functools.wraps(QVTKRenderWindowAdapter.defaultFormat)
    def defaultFormat(
        stereo_capable: bool = False,
    ) -> QtGui.QSurfaceFormat:
        return QVTKRenderWindowAdapter.defaultFormat(stereo_capable)
