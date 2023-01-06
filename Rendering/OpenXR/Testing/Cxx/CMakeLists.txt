# The runtime tests requires an OpenXR runtime and a HMD to pass
# Tested with :
# - Monado on linux
# - Valve Index on Windows TODO LINUX
# - Windows Mixed Reality within SteamVR
option(VTK_OPENXR_TESTING_ENABLE_RUNTIME_TESTS "Enable OpenXR tests that require an OpenXR runtime" OFF)
mark_as_advanced(VTK_OPENXR_TESTING_ENABLE_RUNTIME_TESTS)

# The interactive tests requires an OpenXR runtime and a HMD and will juste run interactively.
# To make it pass, just exit the VR environnement using the menu inside VR.
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
    TestOpenXRInteractiveBox.cxx,NO_VALID
    TestOpenXRInteractiveDragon.cxx,NO_VALID)
endif ()

vtk_add_test_cxx(vtkRenderingOpenXRCxxTests tests ${openxr_tests})
vtk_test_cxx_executable(vtkRenderingOpenXRCxxTests tests)
