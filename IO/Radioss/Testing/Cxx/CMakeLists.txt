vtk_add_test_cxx(vtkIORadiossCxxTests tests
  TestRadiossAnimReader.cxx,NO_VALID,NO_OUTPUT
  )

vtk_test_cxx_executable(vtkIORadiossCxxTests tests)
