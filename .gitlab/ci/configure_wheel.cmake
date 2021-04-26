if (NOT DEFINED "ENV{PYTHON_PREFIX}")
  message(FATAL_ERROR
    "The `PYTHON_PREFIX` environment variable is required.")
endif ()

set(VTK_WHEEL_BUILD ON CACHE BOOL "")
set(CMAKE_PREFIX_PATH "$ENV{PYTHON_PREFIX}" CACHE STRING "")
set(Python3_EXECUTABLE "$ENV{PYTHON_PREFIX}/bin/python" FILEPATH STRING "")
# We always want the Python specified here, not the system one.
set(Python3_FIND_STRATEGY LOCATION CACHE STRING "")

# Official wheels never include remote modules (because they are not under
# VTK's software process).
set(VTK_ENABLE_REMOTE_MODULES OFF CACHE BOOL "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_common.cmake")
