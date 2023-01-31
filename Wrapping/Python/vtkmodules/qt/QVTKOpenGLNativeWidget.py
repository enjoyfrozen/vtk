"""Extends ``QOpenGLWidget`` to make it work with a ``vtkGenericOpenGLRenderWindow``.

Please note that ``QVTKOpenGLNativeWidget`` only works with
``vtkGenericOpenGLRenderWindow``. This is necessary since ``QOpenGLWidget`` wants to take
over the window management as well as the `OpenGL` context creation. Getting that to work
reliably with ``vtkXRenderWindow`` or ``vtkWin32RenderWindow`` (and other platform
specific ``vtkRenderWindow`` subclasses) was tricky and fraught with issues.

Since ``QVTKOpenGLNativeWidget`` uses ``QOpenGLWidget`` to create the ``OpenGL`` context,
it uses ``QSurfaceFormat`` (set using ``QOpenGLWidget.setFormat()`` or
``QSurfaceFormat.setDefaultFormat()``) to create an appropriate window and context.
You can use ``QVTKOpenGLNativeWidget.copyToFormat()`` to obtain a ``QSurfaceFormat``
appropriate for a ``vtkRenderWindow``.

A typical usage for ``QVTKOpenGLNativeWidget`` is as follows:

Example:
    .. highlight:: python
    .. code-block:: python
        from qtpy import QtGui, QtWidgets
        import vtkmodules

        from vtkmodules.vtkRenderingCore import vtkRenderer
        from vtkmodules.qt.QVTKOpenGLNativeWidget import QVTKOpenGLNativeWidget
        from vtkmodules.qt.vtkRenderingOpenGL2 import vtkGenericOpenGLRenderWindow

        class MyWindow(QtWidgets.QMainWindow):
            def __init__(self) -> None:
                QtWidgets.QMainWindow.__init__(self)
                window = vtkGenericOpenGLRenderWindow()

                renderer = vtkRenderer()
                window.AddRenderer(renderer)

                widget = QVTKOpenGLNativeWidget(window)

                # Now continue to use ``window`` as a regular ``vtkRenderWindow``,
                # including adding renderers, actors, etc.

        if __name__ == '__main__':
            QtGui.QSurfaceFormat(QVTKOpenGLNativeWidget.defaultFormat())

            app = QtWidgets.QApplication([])

            win = MyWindow()
            win.show()

            app.exec_()

OpenGL Context:
    In ``QOpenGLWidget`` (superclass for ``QVTKOpenGLNativeWidget``), all rendering
    happens in a frame buffer object (FBO). Thus, care must be taken in the rendering code
    to never directly re-bind the default framebuffer i.e. ID 0.
    ``QVTKOpenGLNativeWidget`` creates an internal ``QOpenGLFramebufferObject``,
    independent of the one created by ``QOpenGLWidget``, for ``vtkRenderWindow`` to do
    rendering. This explicit double-buffering is useful in avoiding temporary
    back buffer only renders done in VTK (e.g. when making selections) from destroying the
    results composed on screen.

Handling Render and Paint:
    The ``Qt`` and ``VTK`` libraries each maintain their own separate ``OpenGL`` contexts.
    Each is double-buffered by default, meaning the front buffer is used to display
    images on screen, the back buffer is drawn to and holds the next image to be
    displayed, and we swap them to show the next image on the screen. Getting ``VTK``
    objects to display in ``Qt`` widgets and windows is the purpose of the ``QVTK``
    modules.

    ``QWidget`` subclasses (including ``QOpenGLWidget`` and ``QVTKOpenGLNativeWidget``)
    display their contents on the screen in ``QWidget.paint()`` in response to a paint
    event. ``QOpenGLWidget`` subclasses are expected to do ``OpenGL`` rendering in
    ``QOpenGLWidget.paintGL()``. ``QWidget`` can receive paint events for various
    reasons including widgets getting or losing focus, some other widget updating on
    the UI e.g. ``QProgressBar`` in status bar updating, etc. If a repaint must be
    triggered outside ``paintGL`` (e.g. when using timers to animate scenes), the widget's
    ``update()`` method `must be called to schedule a repaint`_.

    In ``VTK`` applications, any time the ``vtkRenderWindow`` needs to be updated to
    render a new result, one calls ``vtkRenderWindow.Render()`` on it.
    ``vtkRenderWindowInteractor``s on the render window ensures that, as
    interactions that affect the rendered result happen, it calls ``Render`` on the
    render window.

    Since ``paint`` in ``Qt`` can be called more often then needed, we avoid potentially
    expensive ``vtkRenderWindow.Render()`` calls each time this happens. Instead,
    ``QVTKOpenGLNativeWidget`` relies on the ``VTK`` application calling
    ``vtkRenderWindow.Render()`` on the render window when it needs to update the
    rendering. ``paintGL`` simply passes on the result rendered by the latest render to
    the ``Qt`` windowing system for composing on-screen.

    There may still be occasions when we may have to render in ``paint``, for
    example if the window was resized or ``Qt`` had to recreate the ``OpenGL`` context.
    In this case, ``QVTKOpenGLNativeWidget.paintGL()`` can request a render by calling
    ``QVTKOpenGLNativeWidget.renderVTK``.

    .. _`must be called to schedule a repaint`:
        https://doc.qt.io/qtforpython/PySide6/QtOpenGLWidgets/QOpenGLWidget.html#detailed-description

Initialization and Cleanup:
    ``QVTKOpenGLNativeWidget`` maintains a ``vtkSmartPointer`` to ``RenderWindow`` and a
    ``QScopedPointer`` to ``RenderWindowAdapter``, meaning these member varialbes are
    allocated on the heap and deallocated when they go out of scope.

    In Python, we can achieve the same effect using `context managers`_ (i.e. the
    `__enter__` and `__exit__` magic/dunder methods), but reference counting in Python
    should already handle this well. However, we do need to perform cleanup
    when the ``OpenGL`` context is about to be destroyed. In the ``VTK`` ``C++`` source
    code,  the context's ``aboutToBeDestroyed`` signal is connected to the
    ``cleanupContext``, which calls ``RenderWindowAdapter``'s destructor. So that the
    appropriate cleanup is performed in Python, we instead chain the
    ``QVTKOpenGLNativeWidget`` context's ``aboutToBeDestroyed`` signal to the
    ``RenderWindowAdapter``'s ``aboutToBeDestroyed`` signal. For more info, see
    the `Qt Docs`_.

.. _`Qt Docs`:
    https://doc.qt.io/qtforpython/PySide6/QtOpenGLWidgets/QOpenGLWidget.html#resource-initialization-and-cleanup

Caveats:
    ``QVTKOpenGLNativeWidget`` does not support stereo. Instead, please use
    ``QVTKOpenGLStereoWidget`` if you need support for quad buffer stereo rendering.
    ``QVTKOpenGLNativeWidget`` is targeted for ``Qt`` version ``5.5`` and up.

See Also:
    - ``QVTKOpenGLStereoWidget``
    - ``QVTKRenderWidget``
"""
from __future__ import annotations

import functools

import qtpy
from OpenGL import GL
from qtpy import QtCore, QtGui, QtWidgets
from vtkmodules.vtkInteractionStyle import vtkInteractorStyleTrackballCamera
from vtkmodules.vtkRenderingCore import vtkRenderWindow
from vtkmodules.vtkRenderingOpenGL2 import vtkGenericOpenGLRenderWindow

from .QVTKInteractor import QVTKInteractor
from .QVTKRenderWindowAdapter import QVTKRenderWindowAdapter

if qtpy.API in {'pyqt6', 'pyside6'}:
    from qtpy.QtOpenGLWidgets import QOpenGLWidget
else:
    from qtpy.QtWidgets import QOpenGLWidget


class QVTKOpenGLNativeWidget(QOpenGLWidget):
    def __init__(
        self,
        window: vtkGenericOpenGLRenderWindow,
        parent: QtWidgets.QWidget | None = None,
        f: QtCore.Qt.WindowFlags = QtCore.Qt.WindowFlags(),
    ) -> None:
        """Initialize the ``QVTKOpenGLNativeWidget`` class instance.

        Args:
            window (vtkGenericOpenGLRenderWindow): The render window.
            parent (QtWidgets.QWidget | None, optional): The parent widget. Defaults to None.
            f (QtCore.Qt.WindowFlags, optional): Window flags.
                Defaults to QtCore.Qt.WindowFlags().
        """
        QOpenGLWidget.__init__(self, parent, f)
        self.RenderWindow = window
        # In ``VTK`` C++ source, class member ``RenderWindowAdapter`` is a
        # ``QScopedPointer``, meaning it is allocated on the heap and deallocated when
        # it goes out of scope (like a smart pointer). In Python, we can achieved the
        # same effect in a context manager or manually manage object creation and
        # deletion. We'll adopt the later approach using garbage collection: the adapter
        # is created in ``initializeGL`` and set to ``None`` in appropriate locations.
        self.RenderWindowAdapter = None
        self.EnableHiDPI = True
        self.UnscaledDPI = 72
        self.CustomDevicePixelRatio = 0.0
        self.DefaultCursor = QtCore.Qt.CursorShape.ArrowCursor

        self.setAttribute(QtCore.Qt.WA_Hover)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.setUpdateBehavior(QOpenGLWidget.NoPartialUpdate)
        self.setMouseTracking(True)

        # We use ``QOpenGLWidget.resized()`` instead of ``resizeEvent`` or ``resizeGL`` as
        # an indicator to resize our internal buffer size. This is done, since in
        # addition to widget resize, ``resized`` gets fired when the screen is changed,
        # which causes ``devicePixelRatio`` changes
        self.resized.connect(self.updateSize)

        self.setRenderWindow(window)

        # Enable ``Qt`` gesture events
        self.grabGesture(QtCore.Qt.PinchGesture)
        self.grabGesture(QtCore.Qt.PanGesture)
        self.grabGesture(QtCore.Qt.TapGesture)
        self.grabGesture(QtCore.Qt.TapAndHoldGesture)
        self.grabGesture(QtCore.Qt.SwipeGesture)

    def sizeHint(self):
        return QtCore.QSize(400, 400)

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
        if isinstance(win, vtkRenderWindow):
            win = vtkGenericOpenGLRenderWindow.SafeDownCast(win)
        elif self.RenderWindow is win:
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
            if self.RenderWindow.GetInteractor() is None:
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
                # ``vtkRenderWindow`` is brought to the same state (minus the
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
            self.RenderWindow.GetInteractor() if self.RenderWindow is not None else None
        )

    @QtCore.Slot()
    def cleanupContext(self) -> None:
        """Called as a response to ``QOpenGLContext.aboutToBeDestroyed``.

        This may be called anytime during the widget lifecycle. We need to release any
        ``OpenGL`` resources allocated to ``VTK`` in this method.
        """
        self.RenderWindowAdapter.context().aboutToBeDestroyed.emit()
        self.RenderWindowAdapter = None

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

            self.context().aboutToBeDestroyed.connect(self.cleanupContext)

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
            self.RenderWindowAdapter.blit(
                self.defaultFramebufferObject(),
                GL.GL_COLOR_ATTACHMENT0,
                QtCore.QRect(QtCore.QPoint(0, 0), deviceSize),
            )
            ostate.Pop()
        else:
            # No render window; just fill with white.
            f = QtGui.QOpenGLContext.currentContext().functions()
            f.glClearColor(1.0, 1.0, 1.0, 1.0)
            f.glClear(GL.GL_COLOR_BUFFER_BIT)

    @staticmethod
    @functools.wraps(QVTKRenderWindowAdapter.defaultFormat)
    def defaultFormat(
        stereo_capable: bool = False,
    ) -> QtGui.QSurfaceFormat:
        return QVTKRenderWindowAdapter.defaultFormat(stereo_capable)
