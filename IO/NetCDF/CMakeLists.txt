set(classes
  vtkMPASReader
  vtkNetCDFCAMReader
  vtkNetCDFCFReader
  vtkNetCDFCFWriter
  vtkNetCDFPOPReader
  vtkNetCDFReader
  vtkNetCDFUGRIDReader
  vtkSLACParticleReader
  vtkSLACReader)

vtk_module_add_module(VTK::IONetCDF
  CLASSES ${classes})
vtk_add_test_mangling(VTK::IONetCDF)

# FIXME: vtkSLACParticleReader.cxx:55:31: error: 'int nc_get_vars_longlong(int, int, const size_t*, const size_t*, const ptrdiff_t*, vtkIdType*)' was declared 'extern' and later 'static'
set_property(SOURCE
  vtkSLACParticleReader.cxx
  vtkSLACReader.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
