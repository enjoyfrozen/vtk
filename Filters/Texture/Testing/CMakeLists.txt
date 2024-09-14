if (VTK_WRAP_PYTHON)
  vtk_module_test_data(
    Data/bluntfinq.bin
    Data/bluntfinxyz.bin
    Data/earth.ppm
    Data/masonry.bmp
    Data/texThres2.vtk)

  add_subdirectory(Python)
endif ()

vtk_module_test_data(
  Data/texture_atlas_generator_atlas.vtp
  Data/texture_atlas_generator_mesh.vtp
  )

if (NOT vtk_testing_cxx_disabled)
  add_subdirectory(Cxx)
endif ()
