vtk_module_third_party(
  INTERNAL
    HEADER_ONLY
    LICENSE_FILES "vtkdiy2/LICENSE.txt"
                  "vtkdiy2/LEGAL.txt"
    VERSION       "3.5.0"
    STANDARD_INCLUDE_DIRS
  EXTERNAL
    PACKAGE diy CONFIG_MODE
    TARGETS DIY::diy
    STANDARD_INCLUDE_DIRS)

if (TARGET VTK::mpi)
  vtk_module_link(VTK::diy2
    INTERFACE
      VTK::mpi)

  set(DIY_NO_MPI FALSE)
else ()
  set(DIY_NO_MPI TRUE)
endif ()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/vtk_diy2.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vtk_diy2.h")

vtk_module_install_headers(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/vtk_diy2.h")
