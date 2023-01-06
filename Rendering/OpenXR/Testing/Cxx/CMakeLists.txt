option(VTK_OPENXR_TESTING_ENABLE_RUNTIME_TESTS "Enable OpenXR tests that require an OpenXR runtime" OFF)
mark_as_advanced(VTK_OPENXR_TESTING_ENABLE_RUNTIME_TESTS)
option(VTK_OPENXR_TESTING_ENABLE_INTERACTIVE_TESTS "Enable OpenXR tests that are interactive" OFF) 
mark_as_advanced(VTK_OPENXR_TESTING_ENABLE_INTERACTIVE_TESTS)

set(openxr_tests
  TestOpenXRInstanciation.cxx,NO_VALID)

if (VTK_OPENXR_TESTING_ENABLE_RUNTIME_TESTS)
  list (APPEND openxr_tests
    TestOpenXRInitialization.cxx,NO_VALID
    TestOpenXRRendering.cxx)
endif ()

if (VTK_OPENXR_TESTING_ENABLE_INTERACTIVE_TESTS)
  list (APPEND openxr_tests
    TestOpenXRInteractive.cxx,NO_VALID)
endif ()

vtk_add_test_cxx(vtkRenderingOpenXRCxxTests tests ${openxr_tests})
vtk_test_cxx_executable(vtkRenderingOpenXRCxxTests tests)
