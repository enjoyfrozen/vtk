set(classes
  vtkConduitArrayUtilities
  vtkConduitSource
  vtkDataObjectToConduit)

vtk_module_find_package(PACKAGE catalyst VERSION 2.0)

vtk_module_add_module(VTK::IOCatalystConduit
  CLASSES ${classes})

vtk_module_link(VTK::IOCatalystConduit
  PUBLIC
    catalyst::catalyst)

# Catalyst cannot be mangled since it is a external API, so there is no
# `vtk_catalyst.h` trampoline header. Instead, any other module
# which may bring in its own `libcatalyst` will need to be verified at some
# point (after the top-level `vtk_module_build` is the only reliable place for
# this). To this end, the discovered `catalyst` package is provided here for
# verification elsewhere.
set_property(GLOBAL PROPERTY
  vtk_catalyst_dir_IOCatalystConduit "${catalyst_DIR}")
set_property(GLOBAL PROPERTY
  vtk_catalyst_dir_primary "${catalyst_DIR}")

if (VTK_BUILD_CATALYST)
  add_subdirectory(Catalyst)
endif ()
