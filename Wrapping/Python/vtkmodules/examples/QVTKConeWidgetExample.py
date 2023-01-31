from __future__ import annotations

import sys

import qtpy
import vtkmodules
from qtpy import QtGui, QtWidgets

# VTK 8.2 introduced `vtkmodules` so python programs could import just the modules they
# need to reduce load times. However, VTK factory classes (i.e. those implementing the
# Factory Method design pattern) are in separate modules from their implementation
# classes.
#
# Since it is hard to know modules contain factory classes or implementation classes, or
# which classes need implementations, VTK recommends importing all the important
# implementation modules, even if you may not need them.
#
# - `vtkRenderingOpenGL2`
# - `vtkRenderingFreeType`
# - `vtkInteractionStyle`
# - `vtkRenderingVolumeOpenGL2`
# - `vtkContextOpenGL2`
#
# For details, see:
#  - https://vtk.org/doc/nightly/html/md__builds_gitlab_kitware_sciviz_ci_Documentation_Doxygen_PythonWrappers.html  # pylint: disable=line-too-long
from vtkmodules import (  # noqa: F401  # pylint: disable=unused-import
    vtkInteractionStyle,
    vtkRenderingFreeType,
    vtkRenderingOpenGL2,
)
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

    print('System Info')
    print('-----------')
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
    print()
    print(f'Has alpha: {format_.hasAlpha()}')
    print()


def print_opengl_features() -> None:
    features = {
        'glActiveTexture() Function': QtGui.QOpenGLFunctions.Multitexture,
        'Shader Functions': QtGui.QOpenGLFunctions.Shaders,
        'Vertex and Index Buffers': QtGui.QOpenGLFunctions.Buffers,
        'Framebuffer Object Functions': QtGui.QOpenGLFunctions.Framebuffers,
        'glBlendColor() Function': QtGui.QOpenGLFunctions.BlendColor,
        'glBlendEquation() Function': QtGui.QOpenGLFunctions.BlendEquation,
        'glBlendFuncSeparate() Function': QtGui.QOpenGLFunctions.BlendEquationSeparate,
        'Advanced Blend Equations': QtGui.QOpenGLFunctions.BlendEquationAdvanced,
        'glBlendFuncSeparate()': QtGui.QOpenGLFunctions.BlendFuncSeparate,
        'Blend Subtract Mode': QtGui.QOpenGLFunctions.BlendSubtract,
        'Compressed Texture Functions': QtGui.QOpenGLFunctions.CompressedTextures,
        'glSampleCoverage() Function': QtGui.QOpenGLFunctions.Multisample,
        'Separate Stencil Functions': QtGui.QOpenGLFunctions.StencilSeparate,
        'Non-Power of Two Textures': QtGui.QOpenGLFunctions.NPOTTextures,
        'Non-Power of Two Textures can use GL_REPEAT': QtGui.QOpenGLFunctions.NPOTTextureRepeat,
        'Fixed Function Pipeline': QtGui.QOpenGLFunctions.FixedFunctionPipeline,
        'GL_RED and GL_RG Texture Format': QtGui.QOpenGLFunctions.TextureRGFormats,
        'Multiple Color Attachments': QtGui.QOpenGLFunctions.MultipleRenderTargets,
    }

    context = QtGui.QOpenGLContext.currentContext()
    f = context.functions()

    print('OpenGL Available Features')
    print('-------------------------')

    for key, val in features.items():
        avail = 'YES' if f.hasOpenGLFeature(val) else 'NO'
        print(f'{key}: {avail}')


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
    print_opengl_features()

    app.exec()
