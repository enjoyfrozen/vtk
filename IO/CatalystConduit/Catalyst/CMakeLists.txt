# Requires catalyst SDK
find_package(catalyst 2.0 COMPONENTS SDK REQUIRED)

# Needed by top CMakeLists to check catalyst version consistency across modules
set_property(GLOBAL PROPERTY
  vtk_catalyst_dir_VTKCatalyst "${catalyst_DIR}")

if(WIN32)
  set(catalyst_library_destination
    "${_vtk_build_RUNTIME_DESTINATION}")
else()
  set(catalyst_library_destination
    "${_vtk_build_LIBRARY_DESTINATION}")
endif()

set_property(GLOBAL PROPERTY vtk_catalyst_directory "${catalyst_library_destination}")

catalyst_implementation(
  NAME    vtk
  TARGET  catalyst-vtk
  LIBRARY_DESTINATION
    "${catalyst_library_destination}"
  SOURCES
    VTKCatalyst.cxx
  CATALYST_TARGET catalyst::catalyst)

add_library(VTK::catalyst-vtk ALIAS catalyst-vtk)

# Clear the `-pvVERSION` suffix (if any).
set(_vtk_build_LIBRARY_NAME_SUFFIX "")
# Clear version information.
set(_vtk_build_VERSION "")
set(_vtk_build_SOVERSION "")

_vtk_module_apply_properties(catalyst-vtk)
