set(classes
  vtkMINCImageAttributes
  vtkMINCImageReader
  vtkMINCImageWriter
  vtkMNIObjectReader
  vtkMNIObjectWriter
  vtkMNITagPointReader
  vtkMNITagPointWriter
  vtkMNITransformReader
  vtkMNITransformWriter)

set(headers
  vtkMINC.h)

vtk_module_add_module(VTK::IOMINC
  CLASSES ${classes}
  HEADERS ${headers})
vtk_add_test_mangling(VTK::IOMINC)

# FIXME: vtkMINCImageWriter.cxx:472:20: error: redefinition of 'const char* vtkMINCDimVarNames []'
set_property(SOURCE vtkMINCImageWriter.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
