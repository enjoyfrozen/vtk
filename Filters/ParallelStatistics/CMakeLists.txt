set(classes
  vtkPAutoCorrelativeStatistics
  vtkPBivariateLinearTableThreshold
  vtkPComputeQuantiles
  vtkPComputeQuartiles
  vtkPContingencyStatistics
  vtkPCorrelativeStatistics
  vtkPDescriptiveStatistics
  vtkPKMeansStatistics
  vtkPMultiCorrelativeStatistics
  vtkPOrderStatistics
  vtkPPCAStatistics)

vtk_module_add_module(VTK::FiltersParallelStatistics
  CLASSES ${classes})
vtk_add_test_mangling(VTK::FiltersParallelStatistics)

# FIXME: vtkPOrderStatistics.cxx:47:13: error: redefinition of 'void StringVectorToStringBuffer(const std::vector<std::__cxx11::basic_string<char> >&, std::string&)'
set_property(SOURCE
  vtkPOrderStatistics.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
