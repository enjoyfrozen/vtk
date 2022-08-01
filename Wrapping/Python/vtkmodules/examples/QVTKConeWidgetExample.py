from __future__ import annotations

import sys

import qtpy
import vtkmodules.vtkInteractionStyle  # noqa
import vtkmodules.vtkRenderingOpenGL2  # noqa
from qtpy import QtGui, QtWidgets
from vtkmodules.qt.QVTKOpenGLNativeWidget import QVTKOpenGLNativeWidget
from vtkmodules.vtkFiltersSources import vtkConeSource
from vtkmodules.vtkRenderingCore import vtkActor, vtkPolyDataMapper, vtkRenderer
from vtkmodules.vtkRenderingOpenGL2 import vtkGenericOpenGLRenderWindow

try:
    from OpenGL import GL
except ImportError:
    raise ImportError(
        "PyOpenGL must be installed to run this example. To install, run"
        "'pip install PyOpenGL PyOpenGL_accelerate'"
    )

OS = {
    'win32': 'Windows',
    'darwin': 'MacOS',
    'aix': 'AIX',
    'linux': 'Linux',
    'cygwin': 'Windows with Cygwin',
}


def print_info() -> str:
    python_major, python_minor, python_patch, _, _ = sys.version_info
    os_ = OS.get(sys.platform, sys.platform)

    print(f'OS: {os_}')
    if os_ in {'Windows', 'Windows with Cygwin'}:
        windows_major, windows_minor, windows_build, _, _ = sys.getwindowsversion()
        print(f'OS Version: {windows_major}.{windows_minor}, Build {windows_build}')
    print(f'Python: {python_major}.{python_minor}.{python_patch}')
    print(f'VTK: {vtkmodules.__version__}')
    print(f'Qt Binding: {qtpy.API}')
    print(f'Qt Version: {qtpy.QT_VERSION}')

    context = QtGui.QOpenGLContext.currentContext()
    f = context.functions()
    format_ = context.format()

    gl_vendor = f.glGetString(GL.GL_VENDOR)
    gl_renderer = f.glGetString(GL.GL_RENDERER)
    gl_version = f.glGetString(GL.GL_VERSION)
    gl_lang_version = f.glGetString(GL.GL_SHADING_LANGUAGE_VERSION)

    gl_profile = (
        'Core'
        if format_.profile() is QtGui.QSurfaceFormat.CoreProfile
        else 'Compatibility'
    )
    gl_profile_major = format_.majorVersion()
    gl_profile_minor = format_.minorVersion()

    print(f'Graphics Vendor: {gl_vendor}')
    print(f'Graphics Card: {gl_renderer}')
    print(f'Graphics Driver: {gl_version}')
    print(f'OpenGL Profile: {gl_profile}, version {gl_profile_major}.{gl_profile_minor}')
    print(f'Shader Language: {gl_lang_version}')


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

    print_info()

    app.exec()
