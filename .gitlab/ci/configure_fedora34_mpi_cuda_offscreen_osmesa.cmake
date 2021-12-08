# This has issues with CUDA enable VTK-m
set(VTK_MODULE_ENABLE_VTK_fides NO CACHE STRING "")

# Lowest-common denominator.
set(VTKm_CUDA_Architecture "pascal" CACHE STRING "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_fedora34.cmake")
