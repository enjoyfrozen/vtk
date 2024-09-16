set(classes
  vtkPLY
  vtkPLYReader
  vtkPLYWriter)

vtk_module_add_module(VTK::IOPLY
  CLASSES ${classes})
vtk_add_test_mangling(VTK::IOPLY)

# FIXME: vtkPLYWriter.cxx:215:3: error: reference to 'plyVertex' is ambiguous
set_property(SOURCE vtkPLYWriter.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
