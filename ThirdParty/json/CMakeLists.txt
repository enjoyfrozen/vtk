vtk_module_third_party(
  INTERNAL
    LICENSE_FILES "vtkjson/LICENSE.MIT"
    VERSION       "3.2.0"
    HEADER_ONLY
    STANDARD_INCLUDE_DIRS
  EXTERNAL
    PACKAGE JSON
    TARGETS JSON::JSON
    STANDARD_INCLUDE_DIRS)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/vtk_json_external.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vtk_json_external.h")


vtk_module_install_headers(
  FILES "${CMAKE_CURRENT_SOURCE_DIR}/vtk_json.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/vtk_json_forward.h"
        "${CMAKE_CURRENT_BINARY_DIR}/vtk_json_external.h")
