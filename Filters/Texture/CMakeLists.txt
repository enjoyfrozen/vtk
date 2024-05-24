set(classes
  vtkImplicitTextureCoords
  vtkLSCMFilter
  vtkScalarsToTextureFilter
  vtkTextureMapToCylinder
  vtkTextureMapToPlane
  vtkTextureMapToSphere
  vtkTexturePackingFilter
  vtkThresholdTextureCoords
  vtkTransformTextureCoords
  vtkTriangularTCoords)

vtk_module_add_module(VTK::FiltersTexture
  CLASSES ${classes})
vtk_add_test_mangling(VTK::FiltersTexture)
