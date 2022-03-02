set(classes
  vtkConduitArrayUtilities
  vtkConduitSource
  vtkDataObjectToConduit)

# SDK component is required for Catalyst implementation (see Catalyst subdir)
vtk_module_find_package(PACKAGE catalyst VERSION 2.0 REQUIRED COMPONENTS SDK)

vtk_module_add_module(VTK::IOCatalystConduit
  CLASSES ${classes})

vtk_module_link(VTK::IOCatalystConduit
  PUBLIC
    catalyst::catalyst)

add_subdirectory(Catalyst)
