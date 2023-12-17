set(classes
  vtkHyperTreeGridAxisClip
  vtkHyperTreeGridAxisCut
  vtkHyperTreeGridAxisReflection
  vtkHyperTreeGridCellCenters
  vtkHyperTreeGridContour
  vtkHyperTreeGridDepthLimiter
  vtkHyperTreeGridEvaluateCoarse
  vtkHyperTreeGridExtractGhostCells
  vtkHyperTreeGridFeatureEdges
  vtkHyperTreeGridGeometry
  vtkHyperTreeGridGradient
  vtkHyperTreeGridPlaneCutter
  vtkHyperTreeGridRemoveGhostCells
  vtkHyperTreeGridThreshold
  vtkHyperTreeGridToDualGrid
  vtkHyperTreeGridToUnstructuredGrid
  vtkHyperTreeGridVisibleLeavesSize
  vtkImageDataToHyperTreeGrid
)

set(private_classes
  vtkHyperTreeGridGeometryImpl
  vtkHyperTreeGridGeometry1DImpl
  vtkHyperTreeGridGeometry2DImpl
  vtkHyperTreeGridGeometry3DImpl
  vtkHyperTreeGridGeometrySmallDimensionsImpl
)

vtk_module_add_module(VTK::FiltersHyperTree
  CLASSES ${classes}
  PRIVATE_CLASSES ${private_classes})
vtk_add_test_mangling(VTK::FiltersHyperTree)

# FIXME: vtkHyperTreeGridPlaneCutter.cxx:32:24: error: redefinition of 'constexpr const unsigned int {anonymous}::MooreCursors3D [26]'
set_property(SOURCE vtkHyperTreeGridPlaneCutter.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
