vtk_add_test_cxx(vtkFiltersSolarCxxTests tests
  NO_DATA NO_VALID NO_OUTPUT
  TestEmbreePointLocator.cxx
  )

vtk_test_cxx_executable(vtkFiltersSolarCxxTests tests)
