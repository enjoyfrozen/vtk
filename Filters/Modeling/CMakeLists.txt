set(classes
  vtkAdaptiveSubdivisionFilter
  vtkBandedPolyDataContourFilter
  vtkButterflySubdivisionFilter
  vtkCollisionDetectionFilter
  vtkContourLoopExtraction
  vtkCookieCutter
  vtkDijkstraGraphGeodesicPath
  vtkDijkstraImageGeodesicPath
  vtkFillHolesFilter
  vtkFitToHeightMapFilter
  vtkGeodesicPath
  vtkGraphGeodesicPath
  vtkHausdorffDistancePointSetFilter
  vtkHyperTreeGridOutlineFilter
  vtkImageDataOutlineFilter
  vtkImprintFilter
  vtkLinearCellExtrusionFilter
  vtkLinearExtrusionFilter
  vtkLinearSubdivisionFilter
  vtkLoopSubdivisionFilter
  vtkOutlineFilter
  vtkPolyDataPointSampler
  vtkProjectedTexture
  vtkQuadRotationalExtrusionFilter
  vtkRibbonFilter
  vtkRotationalExtrusionFilter
  vtkRuledSurfaceFilter
  vtkSectorSource
  vtkSelectEnclosedPoints
  vtkSelectPolyData
  vtkSpherePuzzle
  vtkSpherePuzzleArrows
  vtkSubdivideTetra
  vtkTrimmedExtrusionFilter
  vtkVolumeOfRevolutionFilter)

vtk_module_add_module(VTK::FiltersModeling
  CLASSES ${classes})
vtk_add_test_mangling(VTK::FiltersModeling)

# FIXME: vtkCookieCutter.cxx:1262:12: error: no matching function for call to 'sort(std::vector<{anonymous}::SortPoint>::iterator, std::vector<{anonymous}::SortPoint>::iterator, <unresolved overloaded function type>)'
set_property(SOURCE vtkCookieCutter.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
