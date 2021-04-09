find_program(VTK_H5DIFF_EXECUTABLE h5diff)
mark_as_advanced(VTK_H5DIFF_EXECUTABLE)

set(TestHDFConvert_ARGS ${VTK_H5DIFF_EXECUTABLE})
vtk_add_test_python(
  TestHDFConvert.py,NO_VALID
  )
