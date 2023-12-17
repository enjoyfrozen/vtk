set(classes
  vtkArcSource
  vtkArrowSource
  vtkButtonSource
  vtkCapsuleSource
  vtkCellTypeSource
  vtkConeSource
  vtkCubeSource
  vtkCylinderSource
  vtkDiagonalMatrixSource
  vtkDiskSource
  vtkEllipseArcSource
  vtkEllipticalButtonSource
  vtkFrustumSource
  vtkGlyphSource2D
  vtkGraphToPolyData
  vtkHandleSource
  vtkHyperTreeGridPreConfiguredSource
  vtkHyperTreeGridSource
  vtkLineSource
  vtkOutlineCornerFilter
  vtkOutlineCornerSource
  vtkOutlineSource
  vtkParametricFunctionSource
  vtkPartitionedDataSetSource
  vtkPartitionedDataSetCollectionSource
  vtkPlaneSource
  vtkPlatonicSolidSource
  vtkPointHandleSource
  vtkPointSource
  vtkPolyLineSource
  vtkPolyPointSource
  vtkProgrammableDataObjectSource
  vtkProgrammableSource
  vtkRandomHyperTreeGridSource
  vtkRectangularButtonSource
  vtkRegularPolygonSource
  vtkSelectionSource
  vtkSpatioTemporalHarmonicsSource
  vtkSphereSource
  vtkSuperquadricSource
  vtkTessellatedBoxSource
  vtkTextSource
  vtkTexturedSphereSource
  vtkUniformHyperTreeGridSource)

vtk_module_add_module(VTK::FiltersSources
  CLASSES ${classes})
vtk_add_test_mangling(VTK::FiltersSources)

# FIXME: vtkPartitionedDataSetCollectionSource.cxx:53:21: error: redefinition of 'std::pair<int, int> {anonymous}::GetRange(int, const std::vector<int>&)'
set_property(SOURCE vtkPartitionedDataSetCollectionSource.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
