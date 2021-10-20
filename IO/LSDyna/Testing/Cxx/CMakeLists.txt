vtk_add_test_cxx(vtkIOLSDynaCxxTests tests
  TestLSDynaReader.cxx
  TestLSDynaReaderDeflection.cxx
  #TestLSDynaReaderNoDefl.cxx
  TestLSDynaReaderSPH.cxx
  )

vtk_test_cxx_executable(vtkIOLSDynaCxxTests tests)

vtk_add_test_cxx(vtkIOLSDynaCxxTests-NoValid tests
  NO_VALID
  TestLSDynaReaderUserIds.cxx
  )

vtk_test_cxx_executable(vtkIOLSDynaCxxTests-NoValid tests)
