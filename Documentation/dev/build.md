# Building VTK

## Table of Contents

1. [Linux Getting Started](#linux-getting-started)
2. [Obtaining the source](#obtaining-the-source)
3. [Building](#building)
    1. [Prerequisites](#prerequisites)
        1. [Installing CMake](#installing-cmake)
        2. [Installing Qt](#installing-qt)
    2. [Optional Additions](#optional-additions)
        1. [Download And Install ffmpeg movie libraries](#download-and-install-ffmpeg-movie-libraries)
        2. [MPI](#mpi)
        3. [Python](#python)
        4. [OSMesa](#osmesa)
4. [Creating the Build Environment](#creating-the-build-environment)
    1. [Linux (Ubuntu/Debian)](#linux-(ubuntu/debian))
    2. [Windows](#windows)
    3. [WebAssembly (Emscripten)](#webassembly-(emscripten))
5. [Building](#building)
    1. [Missing dependencies](#missing-dependencies)
    2. [Build Settings](#build-settings)
        1. [Mobile devices](#mobile-devices)
        2. [Python wheels](#python-wheels)
6. [Building documentation](#building-documentation)

This page describes how to build and install VTK. It covers building for
development, on both Unix-type systems (Linux, HP-UX, Solaris, macOS), and
Windows. Note that Unix-like environments such as Cygwin and MinGW are not
officially supported. However, patches to fix problems with these platforms
will be considered for inclusion. It is recommended that users which require
VTK to work on these platforms to submit nightly testing results for them.

A full-featured build of VTK depends on several open source tools and libraries
such as Python, Qt, CGNS, HDF5, etc. Some of these are included in the VTK
source itself (e.g., HDF5), while others are expected to be present on the
machine on which VTK is being built (e.g., Python, Qt).

## Linux Getting Started

For new users of VTK or those wanting a quick setup on linux, these instructions will be useful:

* [Getting Started Using Linux](<./getting_started_linux.md>). This will will lead you step by step through the process of setting up VTK in your home folder.

Once you get everything working, don't forget to come back and read the rest of this document.

## Obtaining the source

To obtain VTK's sources locally, clone this repository using
[Git][git].

```sh
git clone --recursive https://gitlab.kitware.com/vtk/vtk.git
```

## Building

VTK supports all of the common generators supported by CMake. The Ninja,
Makefiles, and Visual Studio generators are the most well-tested however.

Note that VTK does not support in-source builds, so you must have a build tree
that is not the source tree.

### Prerequisites

VTK only requires a few packages in order to build in general, however
specific features may require additional packages to be provided to VTK's
build configuration.

Required:

  * [CMake][cmake]
    - Version 3.12 or newer, however, the latest version is always recommended
  * Supported compiler
    - GCC 4.8 or newer
    - Clang 3.3 or newer
    - Apple Clang 7.0 (from Xcode 7.2.1) or newer
    - Microsoft Visual Studio 2015 or newer
    - Intel 14.0 or newer

Optional dependencies:

  * [Python][python]
    - When using Python 3, at least 3.4 is required
  * [Qt5][qt]
    - Version 5.9 or newer

#### Installing CMake

CMake is a tool that makes cross-platform building simple. On several systems
it will probably be already installed or available through system package
management utilities. If it is not, there are precompiled binaries available on
[CMake's download page][cmake-download].

#### Installing Qt

VTK uses Qt as its GUI library (if the relevant modules are enabled).
Precompiled binaries are available on [Qt's website][qt-download].

Note that on Windows, the compiler used for building VTK must match the
compiler version used to build Qt.

### Optional Additions

#### Download And Install ffmpeg movie libraries

When the ability to write `.avi` files is desired, and writing these files is
not supported by the OS, VTK can use the ffmpeg library. This is generally
true for Unix-like operating systems. Source code for ffmpeg can be obtained
from [the website][ffmpeg].

#### MPI

To run VTK in parallel, an [MPI][mpi] implementation is required. If an MPI
implementation that exploits special interconnect hardware is provided on your
system, we suggest using it for optimal performance. Otherwise, on Linux/Mac,
we suggest either [OpenMPI][openmpi] or [MPICH][mpich]. On Windows, [Microsoft
MPI][msmpi] is required.

#### Python

In order to use scripting, [Python][python] is required.

#### OSMesa

Off-screen Mesa can be used as a software-renderer for running VTK on a server
without hardware OpenGL acceleration. This is usually available in system
packages on Linux. For example, the `libosmesa6-dev` package on Debian and
Ubuntu. However, for older machines, building a newer version of Mesa is
likely necessary for bug fixes and support. Its source and build instructions
can be found on [its website][mesa].

## Creating the Build Environment

### Linux (Ubuntu/Debian)

  * `sudo apt install` the following packages:
    - `build-essential`
    - `cmake`
    - `mesa-common-dev`
    - `mesa-utils`
    - `freeglut3-dev`
    - `ninja-build`
      - `ninja` is a speedy replacement for `make`, highly recommended.

### Windows

  * [Visual Studio Community Edition][visual-studio]
  * Use "x64 Native Tools Command Prompt" for the installed Visual Studio
    version to configure with CMake and to build with ninja.
  * Get [ninja][ninja]. Unzip the binary and put it in `PATH`. Note that newer
    Visual Studio releases come with a version of `ninja` already and should
    already exist in `PATH` within the command prompt.

### WebAssembly (Emscripten)

  * [emsdk](https://github.com/emscripten-core/emsdk)
    - Install latest toolchain with `./emsdk install latest`
    - Activate the toolchain `./emsdk activate latest`
    - Run `emsdk_env.bat` or `emsdk_env.ps1` (Windows) or `source ./emsdk_env.sh` (Linux and OS X) to set up the environment for the calling terminal.


## Building

In order to build, CMake requires two steps, configure and build. VTK itself
does not support what are known as in-source builds, so the first step is to
create a build directory.

```sh
mkdir -p vtk/build
cd vtk/build
ccmake ../path/to/vtk/source # -GNinja may be added to use the Ninja generator
```

CMake's GUI has input entries for the build directory and the generator
already. Note that on Windows, the GUI must be launched from a "Native Tools
Command Prompt" available with Visual Studio in the start menu.

### Missing dependencies

CMake may not find all dependencies automatically in all cases. The steps
needed to find any given package depends on the package itself. For general
assistance, please see the documentation for
[`find_package`'s search procedure][cmake-find_package-search] and
[the relevant Find module][cmake-modules-find] (as available).

### Build Settings

VTK has a number of settings available for its build. The common variables
to modify include:

  * `BUILD_SHARED_LIBS` (default `ON`): If set, shared libraries will be
    built. This is usually what is wanted.
  * `VTK_USE_CUDA` (default `OFF`): Whether [CUDA][cuda] support will be available or
    not.
  * `VTK_USE_MPI` (default `OFF`): Whether [MPI][mpi] support will be available or
    not.
  * `VTK_WRAP_PYTHON` (default `OFF`; requires `VTK_ENABLE_WRAPPING`): Whether
    Python support will be available or not.

Less common, but variables which may be of interest to some:

  * `VTK_BUILD_EXAMPLES` (default `OFF`): If set, VTK's example code will be
    added as tests to the VTK test suite.
  * `VTK_ENABLE_LOGGING` (default `ON`): If set, enhanced logging will be
    enabled.
  * `VTK_LOGGING_TIME_PRECISION` (default `3`; requires `VTK_ENABLE_LOGGING`):
    Change the precision of times output when `VTK_ENABLE_LOGGING` is on.
    Possible values are 3 for ms, 6 for us, 9 for ns.
  * `VTK_BUILD_TESTING` (default `OFF`): Whether to build tests or not. Valid
    values are `OFF` (no testing), `WANT` (enable tests as possible), and `ON`
    (enable all tests; may error out if features otherwise disabled are
    required by test code).
  * `VTK_ENABLE_KITS` (default `OFF`; requires `BUILD_SHARED_LIBS`): Compile
    VTK into a smaller set of libraries. Can be useful on platforms where VTK
    takes a long time to launch due to expensive disk access.
  * `VTK_ENABLE_WRAPPING` (default `ON`): Whether any wrapping support will be
    available or not.
  * `VTK_WRAP_JAVA` (default `OFF`; requires `VTK_ENABLE_WRAPPING`):
    Whether Java support will be available or not.
  * `VTK_SMP_IMPLEMENTATION_TYPE` (default `Sequential`): Set which SMPTools
    will be implemented by default. Must be either `Sequential`, `STDThread`,
    `OpenMP` or `TBB`. The backend can be changed at runtime if the desired
    backend has his option `VTK_SMP_ENABLE_<backend_name>` set to `ON`.
  * `VTK_ENABLE_CATALYST` (default `OFF`): Enable the CatlystConduit module
  and build the VTK Catalyst implementation. Depends on an external Catalyst.

OpenGL-related options:

Note that if OpenGL is used, there must be a "sensible" setup. Sanity checks
exist to make sure a broken build is not being made. Essentially:

- at least one rendering environment (X, Cocoa, SDL2, OSMesa, EGL, etc.) must
  be available;
- OSMesa and EGL conflict with each other; and
- OSMesa only supports off-screen rendering and is therefore incompatible with
  Cocoa, X, and SDL2.

  * `VTK_USE_COCOA` (default `ON`; requires macOS): Use Cocoa for
    render windows.
  * `VTK_USE_X` (default `ON` for Unix-like platforms except macOS,
    iOS, and Emscripten, `OFF` otherwise): Use X for render windows.
  * `VTK_USE_SDL2` (default `ON` for Emscripten, `OFF` otherwise): Use
    SDL2 for render windows.
  * `VTK_OPENGL_HAS_OSMESA` (default `OFF`): Use to indicate that the
    OpenGL library being used supports offscreen Mesa rendering
    (OSMesa).
  * `VTK_OPENGL_USE_GLES` (default `OFF`; forced `ON` for Android):
    Whether to use OpenGL ES API for OpenGL or not.
  * `VTK_OPENGL_HAS_EGL` (default `ON` for Android, `OFF` otherwise):
    Use to indicate that the OpenGL library being used supports EGL
    context management.
  * `VTK_DEFAULT_EGL_DEVICE_INDEX` (default `0`; requires
    `VTK_OPENGL_HAS_EGL`): The default EGL device to use for EGL render
    windows.
  * `VTK_ENABLE_WEBGPU` (default `OFF`; required if using Emscripten): Enable
    WebGPU rendering support.
  * `VTK_DEFAULT_RENDER_WINDOW_OFFSCREEN` (default `OFF`): Whether to default
    to offscreen render windows by default or not.
  * `VTK_USE_OPENGL_DELAYED_LOAD` (default `OFF`; requires Windows and CMake >=
    3.13): If set, use delayed loading to load the OpenGL DLL at runtime.
  * `VTK_DEFAULT_RENDER_WINDOW_HEADLESS` (default `OFF`; only available if
    applicable): Default to a headless render window.
  * `VTK_USE_WIN32_OPENGL` (default `ON` for Windows, forced `OFF` otherwise):
    Use Win32 APIs for render windows (typically only relevant for OSMesa on
    Windows builds).

More advanced options:

  * `VTK_ABI_NAMESPACE_NAME` (default `<DEFAULT>` aka `""`): If set, VTK will
     wrap all VTK public symbols in an
     `inline namespace <VTK_ABI_NAMESPACE_NAME>` to allow runtime co-habitation
     with different VTK versions.
  * `VTK_BUILD_DOCUMENTATION` (default `OFF`): If set, VTK will build its API
    documentation using Doxygen.
  * `VTK_BUILD_ALL_MODULES` (default `OFF`): If set, VTK will enable all
    modules not disabled by other features.
  * `VTK_ENABLE_REMOTE_MODULES` (default `ON`): If set, VTK will try to build
    remote modules (the `Remote` directory). If unset, no remote modules will
    build.
  * `VTK_ENABLE_EXTRA_BUILD_WARNINGS` (default `OFF`; requires CMake >= 3.19):
    If set, VTK will enable additional build warnings.
  * `VTK_ENABLE_EXTRA_BUILD_WARNINGS_EVERYTHING` (default `OFF`; requires
    `VTK_ENABLE_EXTRA_BUILD_WARNINGS` and `-Weverything` support): If set, VTK
    will enable all build warnings (with some explicitly turned off).
  * `VTK_USE_EXTERNAL` (default `OFF`): Whether to prefer external third
    party libraries or the versions VTK's source contains.
  * `VTK_TARGET_SPECIFIC_COMPONENTS` (default `OFF`): Whether to install
    files into target-specific components (`<TARGET>-runtime`,
    `<TARGET>-development`, etc.) or general components (`runtime`,
    `development`, etc.)
  * `VTK_VERSIONED_INSTALL` (default `ON`): Whether to add version numbers to
    VTK's include directories and library names in the install tree.
  * `VTK_CUSTOM_LIBRARY_SUFFIX` (default depends on `VTK_VERSIONED_INSTALL`):
    The custom suffix for libraries built by VTK. Defaults to either an empty
    string or `X.Y` where `X` and `Y` are VTK's major and minor version
    components, respectively.
  * `VTK_INSTALL_SDK` (default `ON`): If set, VTK will install its headers,
    CMake API, etc. into its install tree for use.
  * `VTK_FORBID_DOWNLOADS` (default `OFF`): If set, VTK will error on any
    network activity required during the build (namely remote modules and
    testing data).
  * `VTK_DATA_STORE` (default is complicated): If set or detected, points to
    where VTK external data will be stored or looked up.
  * `VTK_DATA_EXCLUDE_FROM_ALL` (default is complicated, but
    generally `OFF`): If set or detected, data downloads will only
    happen upon explicit request rather than through the build's
    default target.
  * `VTK_RELOCATABLE_INSTALL` (default `ON`): If set, the install tree will be
    relocatable to another path. If unset, the install tree may be tied to the
    build machine with absolute paths, but finding dependencies in
    non-standard locations may require work without passing extra information
    when consuming VTK.
  * `VTK_UNIFIED_INSTALL_TREE` (default `OFF`): If set, the install tree is
    stipulated to be a unified install tree of VTK and all of its dependencies;
    a unified tree usually simplifies things including, but not limited to,
    the Python module paths, library search paths, and plugin searching. This
    option is irrelevant if a relocatable install is requested as such setups
    assume that dependencies are set up either via a unified tree or some other
    mechanism such as modules).
  * `VTK_ENABLE_SANITIZER` (default `OFF`): Whether to enable sanitization of
    the VTK codebase or not.
  * `VTK_SANITIZER` (default `address`; requires `VTK_ENABLE_SANITIZER`): The
    sanitizer to use.
  * `VTK_USE_LARGE_DATA` (default `OFF`; requires `VTK_BUILD_TESTING`):
    Whether to enable tests which use "large" data or not (usually used to
    reduce the amount of data downloading required for the test suite).
  * `VTK_USE_HIP` (default `OFF`; requires CMAKE >= 3.21 and NOT `VTK_USE_CUDA`)
    Whether [HIP][hip] support will be available or not.
  * `VTK_LEGACY_REMOVE` (default `OFF`): If set, VTK will disable legacy,
    deprecated APIs.
  * `VTK_LEGACY_SILENT` (default `OFF`; requires `VTK_LEGACY_REMOVE` to be
    `OFF`): If set, usage of legacy, deprecated APIs will not cause warnings.
  * `VTK_USE_FUTURE_CONST` (default `OFF`): If set, the `VTK_FUTURE_CONST`
    macro expands to `const`; otherwise it expands to nothing. This is used to
    incrementally add more const correctness to the codebase while making it
    opt-in for backwards compatibility.
  * `VTK_USE_TK` (default `OFF`; requires `VTK_WRAP_PYTHON`): If set, VTK will
    enable Tkinter support for VTK widgets.
  * `VTK_BUILD_COMPILE_TOOLS_ONLY` (default `OFF`): If set, VTK will compile
    just its compile tools for use in a cross-compile build.
  * `VTK_SERIAL_TESTS_USE_MPIEXEC` (default `OFF`): Used on HPC to run
    serial tests on compute nodes. If set, it prefixes serial tests with
    "${MPIEXEC_EXECUTABLE}" "${MPIEXEC_NUMPROC_FLAG}" "1" ${MPIEXEC_PREFLAGS}
  * `VTK_WINDOWS_PYTHON_DEBUGGABLE` (default `OFF`): Set to `ON` if using a
    debug build of Python.
  * `VTK_WINDOWS_PYTHON_DEBUGGABLE_REPLACE_SUFFIX` (default `OFF`): Set to `ON`
    to use just a `_d` suffix for Python modules.
  * `VTK_BUILD_PYI_FILES` (default `OFF`): Set to `ON` to build `.pyi` type
    hint files for VTK's Python interfaces.
  * `VTK_DLL_PATHS` (default `""` or `VTK_DLL_PATHS` from the environment): If
    set, these paths will be added via Python 3.8's `os.add_dll_directory`
    mechanism in order to find dependent DLLs when loading VTK's Python
    modules. Note that when using the variable, paths are in CMake form (using
    `/`) and in the environment are a path list in the platform's preferred
    format.
  * `VTK_ENABLE_VR_COLLABORATION` (default `OFF`): If `ON`, includes support
    for multi client VR collaboration. Requires libzmq and cppzmq external libraries.
  * `VTK_SMP_ENABLE_<backend_name>` (default `OFF` if needs an external library otherwise `ON`):
    If set, builds with the specified SMPTools backend implementation that can be
    changed on runtime with `VTK_SMP_BACKEND_IN_USE` environment variable.
  * `VTK_USE_VIDEO_FOR_WINDOWS` (default `OFF`; requires Windows): Enable the
    `vtkAVIWriter` class in the `VTK::IOMovie` module.
  * `VTK_USE_VIDEO_FOR_WINDOWS_CAPTURE` (default `OFF`; requires Windows):
    Enable the `vtkWin32VideoSource` class in the `VTK::IOVideo` module.
  * `VTK_USE_MICROSOFT_MEDIA_FOUNDATION` (default `OFF`; requires Windows):
    Enable the `vtkMP4Writer` class in the `VTK::IOMovie` module.
  * `VTK_USE_64BIT_TIMESTAMPS` (default `OFF`; forced on for 64-bit builds):
    Build with 64-bit `vtkMTimeType`.
  * `VTK_USE_64BIT_IDS` (default `OFF` for 32-bit builds; `ON` for 64-bit
    builds): Whether `vtkIdType` should be 32-bit or 64-bit.
  * `VTK_DEBUG_LEAKS` (default `OFF`): Whether VTK will report leaked
    `vtkObject` instances at process destruction or not.
  * `VTK_DEBUG_RANGE_ITERATORS` (default `OFF`; requires a `Debug` build):
    Detect errors with `for-range` iterators in VTK (note that this is very
    slow).
  * `VTK_ALWAYS_OPTIMIZE_ARRAY_ITERATORS` (default `OFF`; requires `NOT
    VTK_DEBUG_RANGE_ITERATORS`): Optimize `for-range` array iterators even in
    `Debug` builds.
  * `VTK_ALL_NEW_OBJECT_FACTORY` (default `OFF`): If `ON`, classes using
    `vtkStandardNewMacro` will use `vtkObjectFactoryNewMacro` allowing
    overrides to be available even when not explicitly requested through
    `vtkObjectFactoryNewMacro` or `vtkAbstractObjectFactoryNewMacro`.
  * `VTK_ENABLE_VTKM_OVERRIDES` (default `OFF`): If `ON`, enables factory override
     of certain VTK filters by their VTK-m counterparts. There is also a runtime
     switch that can be used to enable/disable the overrides at run-time (on by default).
     It can be accessed using the static function `vtkmFilterOverrides::SetEnabled(bool)`.

`vtkArrayDispatch` related options:

The `VTK_DISPATCH_<array_type>_ARRAYS` options (default `OFF` for all but AOS) enable the
specified type of array to be included in a dispatch type list. Explicit arrays (such as
AOS, SOA and Typed) are included in the `vtkArrayDispatchTypeList.h` while
`vtkArrayDispatchImplicitTypeList.h` includes both explicit and implicit arrays. The implicit
array framework is included in the `CommonImplicitArrays` module. The following array types
currently exist for use with the VTK dispatch mechanism:

  * `VTK_DISPATCH_AOS_ARRAYS` (default `ON`): includes dispatching for the commonly used
    "array-of-structure" ordered arrays derived from `vtkAOSDataArrayTemplate`
  * `VTK_DISPATCH_SOA_ARRAYS` (default `OFF`): includes dispatching for "structure-of-array"
    ordered arrays derived from `vtkSOADataArrayTemplate`
  * `VTK_DISPATCH_TYPED_ARRAYS` (default `OFF`): includes dispatching for arrays derived
    from `vtkTypedDataArray`
  * `VTK_DISPATCH_AFFINE_ARRAYS` (default `OFF`): includes dispatching for linearly varying
    `vtkAffineArray`s as part of the implicit array framework
  * `VTK_DISPATCH_CONSTANT_ARRAYS` (default `OFF`): includes dispatching for constant arrays
    `vtkConstantArray` as part of the implicit array framework
  * `VTK_DISPATCH_STD_FUNCTION_ARRAYS` (default `OFF`): includes dispatching for arrays with
    an `std::function` backend `vtkStdFunctionArray` as part of the implicit array framework
  * `VTK_DISPATCH_COMPOSITE_ARRAYS` (default `OFF`): includes dispatching of arrays with a
    `vtkCompositeImplicitBackend` backend, `vtkCompositeArray`, as part of the implicit array
    framework
  * `VTK_DISPATCH_INDEXED_ARRAYS` (default `OFF`): includes dispatching of arrays with a
    `vtkIndexedImplicitBackend` backend, `vtkIndexedArray`, as part of the implicit array
    framework

The outlier in terms of dispatch support is the family of arrays derived from
`vtkScaledSOADataArrayTemplate` which are automatically included in dispatch when built setting
the `VTK_BUILD_SCALED_SOA_ARRAYS`.

> **_WARNING:_**
> Adding increasing numbers of arrays in the dispatch mechanism can greatly slow down compile times.

The VTK module system provides a number of variables to control modules which
are not otherwise controlled by the other options provided.

  * `VTK_MODULE_USE_EXTERNAL_<name>` (default depends on `VTK_USE_EXTERNAL`):
    Use an external source for the named third-party module rather than the
    copy contained within the VTK source tree.

    > **_WARNING:_**
    >
    > Activating this option within an interactive cmake configuration (i.e. ccmake, cmake-gui)
    > could end up finding libraries in the standard locations rather than copies
    > in non-standard locations.
    >
    > It is recommended to pass the variables necessary to find the intended external package to
    > the first configure to avoid finding unintended copies of the external package.
    > The variables which matter depend on the package being found, but those ending with
    > `_LIBRARY` and `_INCLUDE_DIR` as well as the general CMake `find_package` variables ending
    > with `_DIR` and `_ROOT` are likely candidates.
    >
    > ```
    > Example:
    > ccmake -D HDF5_ROOT:PATH=/home/user/myhdf5 ../vtk/sources
    > ```

  * `VTK_MODULE_ENABLE_<name>` (default `DEFAULT`): Change the build settings
    for the named module. Valid values are those for the module system's build
    settings (see below).
  * `VTK_GROUP_ENABLE_<name>` (default `DEFAULT`): Change the default build
    settings for modules belonging to the named group. Valid values are those
    for the module system's build settings (see below).

For variables which use the module system's build settings, the valid values are as follows:

  * `YES`: Require the module to be built.
  * `WANT`: Build the module if possible.
  * `DEFAULT`: Use the settings by the module's groups and
    `VTK_BUILD_ALL_MODULES`.
  * `DONT_WANT`: Don't build the module unless required as a dependency.
  * `NO`: Do not build the module.

If any `YES` module requires a `NO` module, an error is raised.

#### Mobile devices

VTK supports mobile devices in its build. These are triggered by a top-level
flag which then exposes some settings for a cross-compiled VTK that is
controlled from the top-level build.

iOS builds may be enabled by setting the `VTK_IOS_BUILD` option. The following
settings than affect the iOS build:

  * `IOS_SIMULATOR_ARCHITECTURES`
  * `IOS_DEVICE_ARCHITECTURES`
  * `IOS_DEPLOYMENT_TARGET`
  * `IOS_EMBED_BITCODE`

Android builds may be enabled by setting the `VTK_ANDROID_BUILD` option. The
following settings affect the Android build:

  * `ANDROID_NDK`
  * `ANDROID_NATIVE_API_LEVEL`
  * `ANDROID_ARCH_ABI`

#### Python wheels

VTK also supports creating a Python wheel containing its Python wrappers for
Python3. This is supported by setting the `VTK_WHEEL_BUILD` flag. This changes
the build directory structure around to match that expected by wheels. Once
configured, the build tree may be built as it would be normally and then the
generated `setup.py` file used to create the wheel. Note that the `bdist_wheel`
command requires that the `wheel` package is available (`pip install wheel`).

```sh
cmake -GNinja -DVTK_WHEEL_BUILD=ON -DVTK_WRAP_PYTHON=ON path/to/vtk/source
ninja
python3 setup.py bdist_wheel
```

Any modules may be turned on or off as in a normal VTK build. Certain modules
add features to the generated wheel to indicate their availability. These flags
are not meant to be comprehensive, but any reasonable feature flags may be
added to `CMake/vtkWheelFinalization.cmake` as needed.

Note that the wheel will not include any external third party libraries in its
wheel (e.g., X11, OpenGL, etc.) to avoid conflicts with systems or other wheels
doing the same.

##### Modifying Version and/or Distribution Name

When generating a wheel, you can modify the distribution name and/or add a
suffix to the wheel version string.

By default, the distribution name is `vtk` though you can add a suffix via the
`VTK_DIST_NAME_SUFFIX` CMake variable (e.g., set `VTK_DIST_NAME_SUFFIX` to
`'osmesa'` to have the distribution name be `vtk_osmesa`). An underscore (`_`)
character is automatically placed between `vtk` and the value
of `VTK_DIST_NAME_SUFFIX`. Please use `_` characters for further delimination in
the suffix value. Example setting:

```cmake
set(VTK_DIST_NAME_SUFFIX "osmesa" CACHE STRING "")
```

By default (outside of a CI release build), `dev0` is appended to the version of
the package (e.g., `9.2.2.dev0`). This suffix can be controlled through the
`VTK_VERSION_SUFFIX` CMake variable and is useful if generating multiple
wheels and wanting to differentiate the build variants by the version string of
the package.

```cmake
set(VTK_VERSION_SUFFIX "dev0" CACHE STRING "")
```

## Building documentation

The following targets are used to build documentation for VTK:

  * `DoxygenDoc` - build the doxygen documentation from VTK's C++ source files.

[cmake]: https://cmake.org
[cmake-download]: https://cmake.org/download
[cmake-find_package-search]: https://cmake.org/cmake/help/latest/command/find_package.html#search-procedure
[cmake-modules-find]: https://cmake.org/cmake/help/latest/manual/cmake-modules.7.html#find-modules
[cuda]: https://developer.nvidia.com/cuda-zone
[hip]: https://en.wikipedia.org/wiki/ROCm
[ffmpeg]: https://ffmpeg.org
[git]: https://git-scm.org
[mesa]: https://www.mesa3d.org
[mpi]: https://www.mcs.anl.gov/research/projects/mpi
[ninja]: https://ninja-build.org
[msmpi]: https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi
[mpich]: https://www.mpich.org
[nvpipe]: https://github.com/NVIDIA/NvPipe
[openmpi]: https://www.open-mpi.org
[python]: https://python.org
[qt]: https://qt.io
[qt-download]: https://download.qt.io/official_releases/qt
[visual-studio]: https://visualstudio.microsoft.com/vs
