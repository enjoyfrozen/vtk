ExternalData_add_test(vtkConduitCxxTests tests
  NO_VALID NO_OUTPUT
  TestDataObjectToConduit.cxx
  TestConduitSource.cxx
  DATA{../Data/Baseline/bp_amr_example.json,:})

vtk_test_cxx_executable(vtkConduitCxxTests tests)
