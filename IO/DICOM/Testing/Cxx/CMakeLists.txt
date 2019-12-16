set(TEST_SRCS
  TestDICOMCharacterSet.cxx
  TestDICOMDictionary.cxx
  TestDICOMFilePath.cxx
  TestDICOMItem.cxx
  TestDICOMMetaData.cxx
  TestDICOMSequence.cxx
  TestDICOMTagPath.cxx
  TestDICOMUtilities.cxx
  TestDICOMValue.cxx
  TestDICOMVM.cxx
  TestDICOMVR.cxx
)

vtk_add_test_cxx(vtkIODICOMCxxTests TEST_NAMES NO_VALID ${TEST_SRCS})
vtk_test_cxx_executable(vtkIODICOMCxxTests TEST_NAMES)
