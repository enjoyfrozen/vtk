set(classes
  vtkConduitSource
  vtkConduitToDataObject
  vtkDataObjectToConduit)

set(nowrap_classes
  vtkConduitArrayUtilities
)

# SDK component is required for Catalyst implementation (see Catalyst subdir)
vtk_module_find_package(PACKAGE catalyst VERSION 2.0 COMPONENTS SDK)

set_property(GLOBAL PROPERTY vtk_catalyst_dir_primary "${catalyst_DIR}")

vtk_module_add_module(VTK::IOCatalystConduit
  CLASSES ${classes}
  NOWRAP_CLASSES ${nowrap_classes})

vtk_module_link(VTK::IOCatalystConduit
  PUBLIC
    catalyst::catalyst)

if (TARGET vtkm::cuda)
  # Temporarily suppress "has address taken but no possible call to it" warnings,
  # until we figure out its implications.
  # We are disabling all warnings as nvlink has no known way to suppress
  # individual warning types.
  string(APPEND CMAKE_CUDA_FLAGS " -Xnvlink -w")

  list(TRANSFORM nowrap_classes APPEND ".cxx" OUTPUT_VARIABLE nowrap_sources)
  set(cuda_impl ${nowrap_sources})
  set_source_files_properties(${cuda_impl} PROPERTIES LANGUAGE CUDA)

  vtk_module_set_properties(VTK::IOCatalystConduit
    CUDA_SEPARABLE_COMPILATION ON)

  vtk_module_compile_options(VTK::IOCatalystConduit
    PUBLIC $<$<COMPILE_LANGUAGE:CUDA>:-Xcudafe --diag_suppress=extra_semicolon>)
endif ()

vtk_add_test_mangling(VTK::IOCatalystConduit)
add_subdirectory(Catalyst)
