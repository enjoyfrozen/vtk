include("${CMAKE_CURRENT_LIST_DIR}/configure_fedora34.cmake")

set(VTK_USE_CUDA ON CACHE BOOL "")
set(VTK_WRAP_PYTHON OFF CACHE BOOL "")
set(CMAKE_CUDA_COMPILER_LAUNCHER "sccache" CACHE STRING "")

# This has issues with CUDA enable VTK-m
set(VTK_MODULE_ENABLE_VTK_fides NO CACHE STRING "")

# Nothing with Qt
set(VTK_GROUP_ENABLE_Qt "NO" CACHE STRING "")

# Lowest-common denominator.
set(VTKm_CUDA_Architecture "pascal" CACHE STRING "")
