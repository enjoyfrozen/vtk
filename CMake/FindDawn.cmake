#[=======================================================================[.rst:
FindDawn
-----------

Find the dawn native library for webgpu implementation.

IMPORTED Targets
^^^^^^^^^^^^^^^^

This module defines :prop_tgt:`IMPORTED` target ``Dawn::Dawn``, if
dawn has been found.

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``DAWN_FOUND``
  true if dawn headers and libraries were found
``DAWN_INCLUDE_DIR``
  the directory containing webgpu header (webgpu.h)
``DAWN_INCLUDE_DIRS``
  list of the include directories needed to use dawn
``DAWN_LIBRARY``
  dawn library to be linked
``DAWN_VERSION``
  the dawn version
``DAWN_VERSION_MAJOR``
  the dawn major version
``DAWN_VERSION_MINOR``
  the dawn minor version
``DAWN_VERSION_PATCH``
  the dawn patch version

This module reads hints about search locations from variables:
 ``ENV DAWN_ROOT`` or just ``DAWN_ROOT`` - root directory of dawn installation
 ``ENV DAWN_BUILD_DIR`` or just ``DAWN_BUILD_DIR`` - if build directory is different than
                                                     ``${DAWN_ROOT}/build``
 ``ENV DAWN_BUILD_PREFIX`` - specifies the build prefix for user built dawn
                             libraries. Should be specified with ``ENV DAWN_ROOT``
                             and optionally...

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``DAWN_INCLUDE_DIRS``
  list of directory containing dawn webgpu headers (dawn/webgpu.h and webgpu/webgpu.h)
``DAWN_LIBRARY``
  path to the dawn library (dawn_webgpu.dll/libdawn_webgpu.so)
#]=======================================================================]

# ===============================================
# See if we have env vars to help us find wgpu
# ===============================================
set(ENV_DAWN_ROOT $ENV{DAWN_ROOT})
if(DAWN_ROOT AND NOT ENV_DAWN_ROOT)
  set(ENV_DAWN_ROOT ${DAWN_ROOT})
endif()
if(ENV_DAWN_ROOT)
  string(REGEX REPLACE "\\\\" "/" ENV_DAWN_ROOT ${ENV_DAWN_ROOT})
endif()

# initialize search paths
set(DAWN_PREFIX_PATH ${ENV_DAWN_ROOT})
set(DAWN_INC_SEARCH_PATH
  ${ENV_DAWN_ROOT} ${ENV_DAWN_ROOT}/dawn/include)
set(DAWN_LIB_SEARCH_PATH ${ENV_DAWN_ROOT})

# If user built from sources
set(DAWN_BUILD_PREFIX $ENV{DAWN_BUILD_PREFIX})
set(ENV_DAWN_BUILD_DIR $ENV{DAWN_BUILD_DIR})
if(DAWN_BUILD_DIR AND NOT ENV_DAWN_BUILD_DIR)
  set(ENV_DAWN_BUILD_DIR ${DAWN_BUILD_DIR})
endif()
if((DAWN_BUILD_PREFIX OR ENV_DAWN_BUILD_DIR) AND ENV_DAWN_ROOT)
  if(ENV_DAWN_BUILD_DIR)
    string(REGEX REPLACE "\\\\" "/" ENV_DAWN_BUILD_DIR ${ENV_DAWN_BUILD_DIR})
  else()
    set(ENV_DAWN_BUILD_DIR ${ENV_DAWN_ROOT}/build)
  endif()

  # include directory under ${ENV_DAWN_ROOT}/include
  list(APPEND DAWN_LIB_SEARCH_PATH
       ${ENV_DAWN_BUILD_DIR}
       ${ENV_DAWN_BUILD_DIR}/${DAWN_BUILD_PREFIX}_release
       ${ENV_DAWN_BUILD_DIR}/${DAWN_BUILD_PREFIX}_debug)
  list(APPEND DAWN_INC_SEARCH_PATH
      ${ENV_DAWN_BUILD_DIR}/gen/include)
endif()

find_library(
  DAWN_LIBRARY
  NAMES webgpu_dawn
  DOC "Path to webgpu_dawn library"
  HINTS ${DAWN_LIB_SEARCH_PATH}
  PATHS ${DAWN_LIB_SEARCH_PATH})
find_path(
  DAWN_INCLUDE_DIR
  NAMES webgpu/webgpu.h
  DOC "Path to generated webgpu/webgpu.h"
  HINTS ${DAWN_INC_SEARCH_PATH}
  PATHS ${DAWN_INC_SEARCH_PATH})
find_path(
  DAWN_GEN_INCLUDE_DIR
  NAMES dawn/webgpu.h
  DOC "Path to generated dawn/webgpu.h"
  HINTS ${DAWN_INC_SEARCH_PATH}
  PATHS ${DAWN_INC_SEARCH_PATH})
set(DAWN_INCLUDE_DIRS
  ${DAWN_INCLUDE_DIR}
  ${DAWN_GEN_INCLUDE_DIR}
)

if(NOT DAWN_VERSION)
  if(EXISTS "${DAWN_GEN_INCLUDE_DIR}")
    set(DAWN_GEN_SRC "${DAWN_GEN_INCLUDE_DIR}/../src/dawn")
    set(DAWN_VERSION_FILE)
    if (EXISTS "${DAWN_GEN_SRC}/common/Version.h")
      set(DAWN_VERSION_FILE "${DAWN_GEN_SRC}/common/Version.h")
    elseif(EXISTS "${DAWN_GEN_SRC}/common/Version_autogen.h")
      set(DAWN_VERSION_FILE "${DAWN_GEN_SRC}/common/Version_autogen.h")
    endif()
    if(EXISTS ${DAWN_VERSION_FILE})
      file(STRINGS "${DAWN_VERSION_FILE}" DAWN_VERSION_CONTENTS)
      string(REGEX REPLACE "^.*kDawnVersion\\(\"([a-z0-9]+)\".*" "\\1" DAWN_VERSION
        "${DAWN_VERSION_CONTENTS}")
      # Set the version to 0.0.1 for till dawn 1.0 is released
      set(DAWN_VERSION_MAJOR 0)
      set(DAWN_VERSION_MINOR 0)
      set(DAWN_VERSION_PATCH 1)
    else()
      message(AUTHOR_WARNING "Could not find dawn version header file")
      set(DAWN_VERSION "0.0.0")
    endif()
  else()
    message(AUTHOR_WARNING "Could not find dawn version")
    set(DAWN_VERSION "0.0.0")
  endif()
endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Dawn
  REQUIRED_VARS DAWN_INCLUDE_DIRS DAWN_LIBRARY
  VERSION_VAR DAWN_VERSION)
mark_as_advanced(DAWN_INCLUDE_DIRS DAWN_LIBRARY)

if(DAWN_FOUND AND NOT TARGET DAWN::DAWN)
  include(vtkDetectLibraryType)
  vtk_detect_library_type(dawn_library_type PATH "${DAWN_LIBRARY}")
  add_library(DAWN::DAWN "${dawn_library_type}" IMPORTED)
  unset(dawn_library_type)
  set_target_properties(
    DAWN::DAWN
    PROPERTIES IMPORTED_LOCATION "${DAWN_LIBRARY}"
               IMPORTED_IMPLIB "${DAWN_LIBRARY}"
               INTERFACE_INCLUDE_DIRECTORIES "${DAWN_INCLUDE_DIRS}")
endif()
