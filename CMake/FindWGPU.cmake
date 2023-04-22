#[=======================================================================[.rst:
FindWGPU
-----------

Find the wgpu-native library for webgpu implementation.

IMPORTED Targets
^^^^^^^^^^^^^^^^

This module defines :prop_tgt:`IMPORTED` target ``WGPU::WGPU``, if
wgpu has been found.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``WGPU_FOUND``
  true if wgpu headers and libraries were found
``WGPU_INCLUDE_DIR``
the directory containing wgpu-native headers (webgpu.h and wgpu.h)
``WGPU_INCLUDE_DIRS``
  list of the include directories needed to use wgpu
``WGPU_LIBRARY``
  wgpu library to be linked
``WGPU_VERSION``
  the wgpu version
``WGPU_VERSION_MAJOR``
  the wgpu major version
``WGPU_VERSION_MINOR``
  the wgpu minor version
``WGPU_VERSION_PATCH``
  the wgpu patch version

This module reads hints about search locations from variables:
 ``ENV WGPU_ROOT`` or just ``WGPU_ROOT`` - root directory of tbb installation
 ``ENV WGPU_BUILD_PREFIX`` - specifies the build prefix for user built tbb
                        libraries. Should be specified with ``ENV WGPU_ROOT``
                        and optionally...
 ``ENV WGPU_BUILD_DIR`` - if build directory is different than
                        ``${WGPU_ROOT}/build``

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``WGPU_INCLUDE_DIR``
  the directory containing wgpu-native headers
``WGPU_LIBRARY``
  path to the wgpu-native library
#]=======================================================================]

# ===============================================
# See if we have env vars to help us find wgpu
# ===============================================
set(ENV_WGPU_ROOT $ENV{WGPU_ROOT})
if(ENV_WGPU_ROOT)
  string(REGEX REPLACE "\\\\" "/" ENV_WGPU_ROOT ${ENV_WGPU_ROOT})
endif()

# initialize search paths
set(WGPU_PREFIX_PATH ${WGPU_ROOT} ${ENV_WGPU_ROOT})
set(WGPU_INC_SEARCH_PATH ${WGPU_ROOT} ${ENV_WGPU_ROOT})
set(WGPU_LIB_SEARCH_PATH ${WGPU_ROOT} ${ENV_WGPU_ROOT})

# If user built from sources
set(WGPU_BUILD_PREFIX $ENV{WGPU_BUILD_PREFIX})
if(WGPU_BUILD_PREFIX AND ENV_WGPU_ROOT)
  set(ENV_WGPU_BUILD_DIR $ENV{WGPU_BUILD_DIR})
  if(ENV_WGPU_BUILD_DIR)
    string(REGEX REPLACE "\\\\" "/" ENV_WGPU_BUILD_DIR ${ENV_WGPU_BUILD_DIR})
  else()
    set(ENV_WGPU_BUILD_DIR ${ENV_WGPU_ROOT}/build)
  endif()

  # include directory under ${ENV_WGPU_ROOT}/include
  list(APPEND WGPU_LIB_SEARCH_PATH
       ${ENV_WGPU_BUILD_DIR}/${WGPU_BUILD_PREFIX}_release
       ${ENV_WGPU_BUILD_DIR}/${WGPU_BUILD_PREFIX}_debug)
endif()

find_library(
  WGPU_LIBRARY
  NAMES wgpu_native
  PATHS ${WGPU_LIB_SEARCH_PATH})
find_path(
  WGPU_INCLUDE_DIR
  NAMES wgpu.h
  PATHS ${WGPU_INC_SEARCH_PATH})

if(NOT WGPU_VERSION)
  if(EXISTS "${WGPU_INCLUDE_DIR}/commit-sha")
    file(STRINGS "${WGPU_INCLUDE_DIR}/commit-sha" WGPU_VERSION_CONTENTS)
    string(REGEX REPLACE "v(.*)" "\\1" WGPU_VERSION "${WGPU_VERSION_CONTENTS}")
    string(REGEX REPLACE "([0-9]+).(.*)" "\\1" WGPU_VERSION_MAJOR
                         "${WGPU_VERSION}")
    string(REGEX REPLACE "([0-9]+).(.*)" "\\2" WGPU_VERSION_MAJOR_R
                         "${WGPU_VERSION}")
    string(REGEX REPLACE "([0-9]+).(.*)" "\\1" WGPU_VERSION_MINOR
                         "${WGPU_VERSION_MAJOR_R}")
    string(REGEX REPLACE "([0-9]+).(.*)" "\\2" WGPU_VERSION_PATCH
                         "${WGPU_VERSION_MAJOR_R}")
  else()
    message(AUTHOR_WARNING "Could not get WGPU version")
    set(WGPU_VERSION "0.0.0")
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  WGPU
  REQUIRED_VARS WGPU_INCLUDE_DIR WGPU_LIBRARY
  VERSION_VAR WGPU_VERSION)
mark_as_advanced(WGPU_INCLUDE_DIR WGPU_LIBRARY)

if(WGPU_FOUND AND NOT TARGET WGPU::WGPU)
  include(vtkDetectLibraryType)
  vtk_detect_library_type(wgpu_library_type PATH "${WGPU_LIBRARY}")
  add_library(WGPU::WGPU "${wgpu_library_type}" IMPORTED)
  unset(wgpu_library_type)
  set_target_properties(
    WGPU::WGPU
    PROPERTIES IMPORTED_LOCATION "${WGPU_LIBRARY}"
               IMPORTED_IMPLIB "${WGPU_LIBRARY}"
               INTERFACE_INCLUDE_DIRECTORIES "${WGPU_INCLUDE_DIR}")
endif()
