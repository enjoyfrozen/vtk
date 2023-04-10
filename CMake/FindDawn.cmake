#[==[
Provides the following variables:

  * `DAWN_NATIVE_LIBRARY`: Path to Dawn's `native` library.
  * `DAWN_PLATFORM_LIBRARY`: Path to Dawn's `platform` library.
  * `DAWN_PROC_LIBRARY`: Path to Dawn's `proc` library.
  * `DAWN_INCLUDE_DIR`: Directory with Dawn's headers.
  * `DAWN_GEN_INCLUDE_DIR`: Directory with Dawn's generated headers.
  * `DAWN_SRC_INCLUDE_DIR`: Directory with Dawn's generated sources.
#]==]

find_library(DAWN_NATIVE_LIBRARY
  NAMES
    dawn_native
  PATHS ${DAWN_BINARY_DIR}
  DOC "Path to libdawn_native")
mark_as_advanced(DAWN_NATIVE_LIBRARY)

find_library(DAWN_PLATFORM_LIBRARY
  NAMES
    dawn_platform
  PATHS ${DAWN_BINARY_DIR}
  DOC "Path to libdawn_platform")
mark_as_advanced(DAWN_PLATFORM_LIBRARY)

find_library(DAWN_PROC_LIBRARY
  NAMES
    dawn_proc
  PATHS ${DAWN_BINARY_DIR}
  DOC "Path to libdawn_proc")
mark_as_advanced(DAWN_PROC_LIBRARY)

find_path(DAWN_GEN_INCLUDE_DIR
  NAMES dawn/webgpu_cpp.h
  PATHS ${DAWN_BINARY_DIR}/gen/include
  DOC "Path to generated dawn/webgpu_cpp.h")
mark_as_advanced(DAWN_GEN_INCLUDE_DIR)

find_path(DAWN_GEN_SRC_DIR
  NAMES dawn/webgpu_cpp.cpp
  PATHS ${DAWN_BINARY_DIR}/gen/src
  DOC "Path to generated dawn/webgpu_cpp.cpp")
mark_as_advanced(DAWN_GEN_SRC_DIR)

find_path(DAWN_INCLUDE_DIR
  NAMES dawn/dawn_proc.h
  PATHS ${DAWN_SOURCE_DIR}/include
  DOC "Path to generated dawn/dawn_proc.h")
mark_as_advanced(DAWN_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Dawn
  REQUIRED_VARS
    DAWN_INCLUDE_DIR DAWN_GEN_INCLUDE_DIR DAWN_GEN_SRC_DIR
    DAWN_PLATFORM_LIBRARY DAWN_NATIVE_LIBRARY DAWN_PROC_LIBRARY)
