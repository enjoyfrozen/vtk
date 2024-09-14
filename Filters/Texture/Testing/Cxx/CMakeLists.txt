#vtk_add_test_cxx(vtkFiltersTextureCxxTests tests
  #  TestScalarsToTexture.cxx
  #  )

vtk_add_test_cxx(vtkFiltersTextureCxxTests tests
  NO_RT NO_VALID
  TestTextureAtlasGenerator.cxx
  )

vtk_test_cxx_executable(vtkFiltersTextureCxxTests tests)
