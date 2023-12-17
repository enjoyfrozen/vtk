set(classes
  LSDynaFamily
  LSDynaMetaData
  vtkLSDynaReader
  vtkLSDynaSummaryParser)

set(nowrap_classes
  vtkLSDynaPart
  vtkLSDynaPartCollection)

vtk_module_add_module(VTK::IOLSDyna
  CLASSES ${classes}
  NOWRAP_CLASSES ${nowrap_classes})
vtk_add_test_mangling(VTK::IOLSDyna)

# FIXME: vtkLSDynaSummaryParser.cxx:15:6: error: redefinition of 'void {anonymous}::vtkLSTrimWhitespace(std::string&)'
set_property(SOURCE vtkLSDynaSummaryParser.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
