#[==[
Uses the following variables:

  * `DAWN_NATIVE_LIBRARY`: Path to Dawn's `native` library.
  * `DAWN_PLATFORM_LIBRARY`: Path to Dawn's `platform` library.
  * `DAWN_CPP_LIBRARY`: Path to Dawn's `cpp` library.
  * `DAWN_PROC_LIBRARY`: Path to Dawn's `proc` library.
  * `DAWN_INCLUDE_DIR`: Directory with Dawn's headers.
  * `DAWN_GEN_INCLUDE_DIR`: Directory with Dawn's generated headers.

Imported targets:

  * `Dawn::Headers`: `INTERFACE` target for Dawn's headers.
  * `Dawn::Platform`: Target for `dawn_platform`
  * `Dawn::Proc`: Target for `dawn_proc`
  * `Dawn::Native`: Target for `dawn_native`
#]==]

find_library(DAWN_NATIVE_LIBRARY
  NAMES
    dawn_native
  PATHS ${DAWN_BINARY_DIR}
  PATH_SUFFIXES
    src/dawn/native
  DOC "Path to dawn_native library")
mark_as_advanced(DAWN_NATIVE_LIBRARY)

find_library(DAWN_PLATFORM_LIBRARY
  NAMES
    dawn_platform
  PATHS ${DAWN_BINARY_DIR}
  PATH_SUFFIXES
    src/dawn/platform
  DOC "Path to dawn_platform library")
mark_as_advanced(DAWN_PLATFORM_LIBRARY)

find_library(DAWN_CPP_LIBRARY
  NAMES
    dawncpp
  PATHS ${DAWN_BINARY_DIR}
  PATH_SUFFIXES
    src/dawn
  DOC "Path to dawncpp library")
mark_as_advanced(DAWN_CPP_LIBRARY)

find_library(DAWN_PROC_LIBRARY
  NAMES
    dawn_proc
  PATHS ${DAWN_BINARY_DIR}
  PATH_SUFFIXES
    src/dawn
  DOC "Path to dawn_proc library")
mark_as_advanced(DAWN_PROC_LIBRARY)

find_path(DAWN_GEN_INCLUDE_DIR
  NAMES dawn/webgpu_cpp.h
  PATHS ${DAWN_BINARY_DIR}
  PATH_SUFFIXES
    gen/include
  DOC "Path to generated dawn/webgpu_cpp.h")
mark_as_advanced(DAWN_GEN_INCLUDE_DIR)

find_path(DAWN_INCLUDE_DIR
  NAMES dawn/dawn_proc.h
  PATHS ${DAWN_SOURCE_DIR}/include
  DOC "Path to generated dawn/dawn_proc.h")
mark_as_advanced(DAWN_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Dawn
  REQUIRED_VARS
    DAWN_INCLUDE_DIR DAWN_GEN_INCLUDE_DIR
    DAWN_PLATFORM_LIBRARY DAWN_NATIVE_LIBRARY DAWN_CPP_LIBRARY DAWN_PROC_LIBRARY)

if (Dawn_FOUND)
  if (NOT TARGET Dawn::Headers)
    add_library(Dawn::Headers INTERFACE IMPORTED)
    set_target_properties(Dawn::Headers PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${DAWN_INCLUDE_DIR};${DAWN_GEN_INCLUDE_DIR}")
  endif ()
  if (NOT TARGET Dawn::Platform)
    add_library(Dawn::Platform UNKNOWN IMPORTED)
    set_target_properties(Dawn::Platform PROPERTIES
      IMPORTED_LOCATION "${DAWN_PLATFORM_LIBRARY}"
      INTERFACE_LINK_LIBRARIES "Dawn::Headers")
  endif ()
  if (NOT TARGET Dawn::Native)
    add_library(Dawn::Native UNKNOWN IMPORTED)
    set_target_properties(Dawn::Native PROPERTIES
      IMPORTED_LOCATION "${DAWN_NATIVE_LIBRARY}"
      INTERFACE_LINK_LIBRARIES "Dawn::Headers;Dawn::Platform")
  endif ()
  if (NOT TARGET Dawn::Cpp)
    add_library(Dawn::Cpp UNKNOWN IMPORTED)
    set_target_properties(Dawn::Cpp PROPERTIES
      IMPORTED_LOCATION "${DAWN_CPP_LIBRARY}"
      INTERFACE_LINK_LIBRARIES "Dawn::Headers")
  endif ()
  if (NOT TARGET Dawn::Proc)
    add_library(Dawn::Proc UNKNOWN IMPORTED)
    set_target_properties(Dawn::Proc PROPERTIES
      IMPORTED_LOCATION "${DAWN_PROC_LIBRARY}"
      INTERFACE_LINK_LIBRARIES "Dawn::Headers")
  endif ()
endif ()
