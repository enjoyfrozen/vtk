set(classes
  vtkCMLMoleculeReader
  vtkGaussianCubeReader
  vtkGaussianCubeReader2
  vtkMoleculeReaderBase
  vtkPDBReader
  vtkVASPAnimationReader
  vtkVASPTessellationReader
  vtkXYZMolReader
  vtkXYZMolReader2
)

vtk_module_add_module(VTK::IOChemistry
  CLASSES ${classes}
)
vtk_add_test_mangling(VTK::IOChemistry)

# FIXME: vtkVASPTessellationReader.cxx:47:6: error: redefinition of 'template<class T> bool {anonymous}::parse(const std::string&, T&)'
set_property(SOURCE vtkVASPTessellationReader.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
