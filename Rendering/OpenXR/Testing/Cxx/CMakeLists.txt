vtk_add_test_cxx(vtkRenderingOpenXRCxxTests tests
  TestOpenXRInstanciation.cxx,NO_VALID
  TestOpenXRInitialization.cxx,NO_VALID
  TestOpenXRRendering.cxx
  TestOpenXRInteractive.cxx,NO_VALID
  )

vtk_test_cxx_executable(vtkRenderingOpenXRCxxTests tests)
