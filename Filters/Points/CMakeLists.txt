set(classes
  vtkBoundedPointSource
  vtkConnectedPointsFilter
  vtkConvertToPointCloud
  vtkDensifyPointCloudFilter
  vtkEllipsoidalGaussianKernel
  vtkEuclideanClusterExtraction
  vtkExtractEnclosedPoints
  vtkExtractHierarchicalBins
  vtkExtractPointCloudPiece
  vtkExtractPoints
  vtkExtractSurface
  vtkFitImplicitFunction
  vtkGaussianKernel
  vtkGeneralizedKernel
  vtkHierarchicalBinningFilter
  vtkInterpolationKernel
  vtkLinearKernel
  vtkMaskPointsFilter
  vtkPCACurvatureEstimation
  vtkPCANormalEstimation
  vtkPointCloudFilter
  vtkPointDensityFilter
  vtkPointInterpolator
  vtkPointInterpolator2D
  vtkPointOccupancyFilter
  vtkPointSmoothingFilter
  vtkPoissonDiskSampler
  vtkProbabilisticVoronoiKernel
  vtkProjectPointsToPlane
  vtkRadiusOutlierRemoval
  vtkSPHCubicKernel
  vtkSPHInterpolator
  vtkSPHKernel
  vtkSPHQuarticKernel
  vtkSPHQuinticKernel
  vtkShepardKernel
  vtkSignedDistance
  vtkStatisticalOutlierRemoval
  vtkUnsignedDistance
  vtkVoronoiKernel
  vtkVoxelGrid
  vtkWendlandQuinticKernel)

vtk_module_add_module(VTK::FiltersPoints
  CLASSES ${classes})
vtk_add_test_mangling(VTK::FiltersPoints)

# FIXME: vtkPointInterpolator2D.cxx:94:8: error: redefinition of 'struct {anonymous}::ProbePoints'
set_property(SOURCE
  vtkPointInterpolator2D.cxx
  vtkFitImplicitFunction.cxx
  vtkSPHInterpolator.cxx
  vtkMaskPointsFilter.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
