set(classes
  vtkBiomTableReader
  vtkChacoGraphReader
  vtkDelimitedTextReader
  vtkDIMACSGraphReader
  vtkDIMACSGraphWriter
  vtkFixedWidthTextReader
  vtkISIReader
  vtkMultiNewickTreeReader
  vtkNewickTreeReader
  vtkNewickTreeWriter
  vtkPhyloXMLTreeReader
  vtkPhyloXMLTreeWriter
  vtkRISReader
  vtkTemporalDelimitedTextReader
  vtkTulipReader
  vtkXGMLReader
  vtkXMLTreeReader)

vtk_module_add_module(VTK::IOInfovis
  CLASSES ${classes})
vtk_add_test_mangling(VTK::IOInfovis)

# FIXME: vtkTulipReader.cxx:40:12: error: ambiguating new declaration of 'int my_getline(std::istream&, std::string&, char)'
set_property(SOURCE
  vtkTulipReader.cxx
  vtkXGMLReader.cxx
  vtkFixedWidthTextReader.cxx
  vtkISIReader.cxx
  vtkRISReader.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
