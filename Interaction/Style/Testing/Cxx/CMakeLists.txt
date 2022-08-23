set(private_classes
  vtkTrackballStyleTestUtils)

vtk_add_test_cxx(vtkInteractionStyleCxxTests tests
  TestTrackballStyle.cxx
  TestTrackballConstrainedStyle.cxx
  TestTrackballSingularityStyle.cxx
  TestTrackballZoom.cxx
  TestTrackballZoomTargetted.cxx
  )
vtk_test_cxx_executable(vtkInteractionStyleCxxTests tests
  RENDERING_FACTORY
  )
