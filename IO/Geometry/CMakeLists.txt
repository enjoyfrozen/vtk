set(classes
  vtkAVSucdReader
  vtkBYUReader
  vtkBYUWriter
  #vtkCGMWriter # Needs vtkViewport.
  vtkChacoReader
  vtkFacetWriter
  vtkFLUENTReader
  vtkGAMBITReader
  vtkGLTFDocumentLoader
  vtkGLTFReader
  vtkGLTFTexture
  vtkGLTFWriter
  vtkHoudiniPolyDataWriter
  vtkIVWriter
  vtkMCubesReader
  vtkMCubesWriter
  vtkMFIXReader
  vtkOBJReader
  vtkOBJWriter
  vtkOFFReader
  vtkOpenFOAMReader
  vtkParticleReader
  vtkProStarReader
  vtkPTSReader
  vtkSTLReader
  vtkSTLWriter
  vtkTecplotReader
  vtkWindBladeReader)

set(private_classes
  vtkGLTFDocumentLoaderInternals
  vtkGLTFWriterUtils
  vtkGLTFUtils)

set(headers
  GLTFSampler.h)

vtk_module_add_module(VTK::IOGeometry
  CLASSES ${classes}
  PRIVATE_CLASSES ${private_classes}
  HEADERS ${headers}
  )
vtk_add_test_mangling(VTK::IOGeometry)

# FIXME: vtkGLTFReader.cxx:35:13: error: redefinition of 'template<class T> std::string {anonymous}::value_to_string(const T&)'
set_property(SOURCE
  vtkGLTFReader.cxx
  vtkGLTFDocumentLoaderInternals.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
