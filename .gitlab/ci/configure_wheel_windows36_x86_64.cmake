# The CI script adds the required path to `PATH`.
set(vtk_python_interpreter_can_import_vtk ON CACHE STRING "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_wheel.cmake")
