from __future__ import annotations

import vtkmodules.vtkInteractionStyle  # noqa
import vtkmodules.vtkRenderingOpenGL2  # noqa
from qtpy import QtGui, QtWidgets
from vtkmodules.qt.QVTKOpenGLNativeWidget import QVTKOpenGLNativeWidget
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkRenderingCore import vtkActor, vtkPolyDataMapper, vtkRenderer
from vtkmodules.vtkRenderingOpenGL2 import vtkGenericOpenGLRenderWindow


class ConeWidgetExample(QtWidgets.QMainWindow):
    def __init__(self) -> None:

        QtWidgets.QMainWindow.__init__(self)

        cone = vtkConeSource()
        cone.SetResolution(8)

        coneMapper = vtkPolyDataMapper()
        coneMapper.SetInputConnection(cone.GetOutputPort())

        coneActor = vtkActor()
        coneActor.SetMapper(coneMapper)

        renderer = vtkRenderer()
        renderer.AddActor(coneActor)

        self.window = vtkGenericOpenGLRenderWindow()
        self.window.AddRenderer(renderer)

        self.widget = QVTKOpenGLNativeWidget(self.window)
        self.widget.setRenderWindow(self.window)

        self.setCentralWidget(self.widget)


if __name__ == '__main__':
    # Calling ``setDefaultFormat()`` before constructing the ``QApplication`` instance
    # is mandatory on some platforms (for example, macOS) when an ``OpenGL`` core profile
    # context is requested. This is to ensure that resource sharing between contexts stays
    # functional as all internal contexts are created using the correct version and
    # profile.
    # See:
    #  - https://doc.qt.io/qtforpython/PySide6/QtOpenGLWidgets/QOpenGLWidget.html#painting-techniques
    QtGui.QSurfaceFormat.setDefaultFormat(QVTKOpenGLNativeWidget.defaultFormat())
    app = QtWidgets.QApplication([])
    win = ConeWidgetExample()
    win.show()

    app.exec()
