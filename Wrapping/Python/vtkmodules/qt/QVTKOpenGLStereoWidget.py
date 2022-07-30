"""Extends ``QOpenGLWidget`` to make it work with a ``vtkGenericOpenGLRenderWindow`` with quad buffer stereo rendering.

``QVTKOpenGLStereoWidget`` simplifies using a ``QVTKOpenGLWindow`` as a widget in ``Qt``
application so it can be embedded in a layout rather than being a top-level
window. ``QVTKOpenGLWindow`` has all the limitations posed by ``Qt`` with
``QWidget.createWindowContainer()`` hence developers are advised to refer to ``Qt``
docs for more details.

In general ``QVTKOpenGLNativeWidget`` may be a better choice, however
``QVTKOpenGLWindow``-based ``QVTKOpenGLStereoWidget`` may be better choice for
applications requiring quad-buffer stereo.

Due to ``Qt`` limitations, ``QVTKOpenGLStereoWidget`` does not support being a
native widget. But native widget are sometimes mandatory, for example within
``QScrollArea`` and ``QMDIArea``, so the ``QVTKOpenGLNativeWidget`` should be
used when in needs of VTK rendering in the context of ``Qt`` native widget.

If a ``QVTKOpenGLStereoWidget`` is used in a ``QScrollArea`` or in a ``QMDIArea``, it
will force it to be native and this is **NOT** supported.

Unlike ``QVTKOpenGLNativeWidget``, ``QVTKOpenGLStereoWidget`` does not require that the
default surface format for the application be changed. One can simply specify
the needed ``QSurfaceFormat`` for the specific ``QVTKOpenGLStereoWidget`` instance by
calling ``QVTKOpenGLStereoWidget.setFormat()`` before the widget is initialized.
"""

from __future__ import annotations

import functools

from qtpy import QtCore, QtGui, QtWidgets
from QVTKInteractor import QVTKInteractor
from QVTKOpenGLWindow import QVTKOpenGLWindow
from QVTKRenderWindowAdapter import QVTKRenderWindowAdapter
from vtkmodules.vtkRenderingCore import vtkRenderWindow
from vtkmodules.vtkRenderingOpenGL2 import vtkGenericOpenGLRenderWindow


class QVTKOpenGLStereoWidget(QtWidgets.QWidget):
    def __init__(
        self,
        parent: QtWidgets.QWidget | None = None,
        updateBehavior: QtGui.QOpenGLWindow.UpdateBehavior = QtGui.QOpenGLWindow.NoPartialUpdate,
        f: QtCore.Qt.WindowFlags = QtCore.Qt.WindowFlags(),
        shareContext: QtGui.QOpenGLContext | None = None,
        renderWindow: vtkGenericOpenGLRenderWindow | None = None,
    ) -> None:
        QtWidgets.QWidget.__init__(self, parent, f)

        self.vBoxLayout = QtWidgets.QVBoxLayout(self)
        self.vBoxLayout.setContentsMargins(0, 0, 0, 0)

        if renderWindow is None:
            renderWindow = vtkGenericOpenGLRenderWindow()

        if shareContext is None:
            shareContext = QtGui.QOpenGLContext.currentContext()

        self.__VTKOpenGLWindow = QVTKOpenGLWindow(
            renderWindow,
            shareContext,
        )

        self.container: QtWidgets.QWidget = QtWidgets.QWidget.createWindowContainer(
            self.__VTKOpenGLWindow,
            self,
            f,
        )
        self.container.setAttribute(QtCore.Qt.WA_TransparentForMouseEvents)
        self.container.setMouseTracking(True)
        self.vBoxLayout.addWidget(self.container)

        self.__VTKOpenGLWindow.windowEvent.connect(
            functools.partial(QtWidgets.QApplication.sendEvent, self)
        )

        self.setMouseTracking(True)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)

        # Workaround for bug paraview/paraview#18285
        # https://gitlab.kitware.com/paraview/paraview/-/issues/18285
        # This ensures kde will not grab the window
        self.setProperty('_kde_no_window_grab', True)

        self.grabGesture(QtCore.Qt.PinchGesture)
        self.grabGesture(QtCore.Qt.PanGesture)
        self.grabGesture(QtCore.Qt.TapGesture)
        self.grabGesture(QtCore.Qt.TapAndHoldGesture)
        self.grabGesture(QtCore.Qt.SwipeGesture)

    @functools.wraps(QVTKOpenGLWindow.setRenderWindow)
    def setRenderWindow(
        self,
        win: vtkRenderWindow | vtkGenericOpenGLRenderWindow,
    ) -> None:
        self.__VTKOpenGLWindow.setRenderWindow(win)

    @functools.wraps(QVTKOpenGLWindow.renderWindow)
    def renderWindow(self) -> vtkRenderWindow:
        return self.__VTKOpenGLWindow.renderWindow()

    @functools.wraps(QVTKOpenGLWindow.enableHiDPI)
    def enableHiDPI(self) -> bool:
        return self.__VTKOpenGLWindow.enableHiDPI()

    @functools.wraps(QVTKOpenGLWindow.setEnableHiDPI)
    def setEnableHiDPI(
        self,
        enable: bool,
    ) -> None:
        return self.__VTKOpenGLWindow.setEnableHiDPI(enable)

    def unscaledDPI(self) -> int:
        return self.UnscaledDPI

    @functools.wraps(QVTKOpenGLWindow.setUnscaledDPI)
    def setUnscaledDPI(self, dpi: int) -> None:
        self.__VTKOpenGLWindow.setUnscaledDPI(dpi)

    @functools.wraps(QVTKOpenGLWindow.setCustomDevicePixelRatio)
    def setCustomDevicePixelRatio(
        self,
        cdpr: float,
    ) -> None:
        return self.__VTKOpenGLWindow.setCustomDevicePixelRatio(cdpr)

    @functools.wraps(QVTKOpenGLWindow.customDevicePixelRatio)
    def customDevicePixelRatio(self) -> float:
        return self.__VTKOpenGLWindow.CustomDevicePixelRatio

    @functools.wraps(QVTKOpenGLWindow.effectiveDevicePixelRatio)
    def effectiveDevicePixelRatio(self) -> float:
        return self.__VTKOpenGLWindow.effectiveDevicePixelRatio()

    @functools.wraps(QVTKOpenGLWindow.setDefaultCursor)
    def setDefaultCursor(
        self,
        cursor: QtCore.QCursor,
    ) -> None:
        return self.__VTKOpenGLWindow.setDefaultCursor(cursor)

    def defaultCursor(self) -> QtCore.QCursor:
        return self.__VTKOpenGLWindow.defaultCursor()

    @functools.wraps(QVTKOpenGLWindow.interactor)
    def interactor(self) -> QVTKInteractor:
        return self.__VTKOpenGLWindow.interactor()

    def isValid(self) -> bool:
        return self.__VTKOpenGLWindow.isValid()

    def grabFramebuffer(self) -> QtGui.QImage:
        """Expose internal ``QVTKOpenGlWindow.grabFramebuffer()``.

        Renders and returns a 32-bit RGB image of the framebuffer.

        Returns:
            QtGui.QImae: The frame buffer.
        """
        return self.__VTKOpenGLWindow.grabFramebuffer()

    def embeddedOpenGLWindow(self) -> QVTKOpenGLWindow:
        """Return the embedded ``QVTKOpenGLWindow``.

        Returns:
            QVTKOpenGLWindow: The embedded ``QVTKOpenGLWindow``
        """
        return self.__VTKOpenGLWindow

    def setFormat(
        self,
        fmt: QtGui.QSurfaceFormat,
    ) -> None:
        """Set the requested surface format.

        When the format is not explicitly set via this function, the format returned by
        ``QSurfaceFormat.defaultFormat()`` will be used. This means that when having
        multiple ``OpenGL`` widgets, individual calls to this function can be replaced by
        one single call to ``QSurfaceFormat.setDefaultFormat()`` before creating the first
        widget.

        Args:
            fmt (QtGui.QSurfaceFormat): The surface format.
        """
        return self.__VTKOpenGLWindow.setFormat(fmt)

    def format(self) -> QtGui.QSurfaceFormat:
        """Return the context and surface format used.

        Returns the context and surface format used by this widget and its toplevel
        window.

        Returns:
            QtGui.QSurfaceFormat: The surface format used.
        """
        return self.__VTKOpenGLWindow.format()

    @staticmethod
    @functools.wraps(QVTKRenderWindowAdapter.defaultFormat)
    def defaultFormat(
        stereo_capable: bool = False,
    ) -> QtGui.QSurfaceFormat:
        return QVTKRenderWindowAdapter.defaultFormat(stereo_capable)

    def resizeEvent(
        self,
        evt: QtGui.QResizeEvent,
    ) -> None:
        super().resizeEvent(evt)

    def paintEvent(
        self,
        evt: QtGui.QPaintEvent,
    ) -> None:
        super().paintEvent(evt)

        # This is generally not needed. However, there are cases where after a resize the
        # embedded ``QVTKOpenGLWindow`` doesn't repaint (even though it correctly
        # receives the resize event). Explicitly triggering update on the internal widget
        # overcomes this issue.
        self.__VTKOpenGLWindow.update()
