# `isnan` returns `0` for values which print `nan`
set(VTK_HAS_STD_ISNAN FALSE CACHE BOOL "")
set(VTK_HAS_ISNAN FALSE CACHE BOOL "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_el8.cmake")
