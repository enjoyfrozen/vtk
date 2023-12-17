set(classes
  vtkAnnotatedCubeActor
  vtkArcPlotter
  vtkAxesActor
  vtkAxisActor
  vtkAxisActor2D
  vtkAxisFollower
  vtkBarChartActor
  vtkCaptionActor2D
  vtkConvexHull2D
  vtkCornerAnnotation
  vtkCubeAxesActor
  vtkCubeAxesActor2D
  vtkGraphAnnotationLayersFilter
  vtkLeaderActor2D
  vtkLegendBoxActor
  vtkLegendScaleActor
  vtkParallelCoordinatesActor
  vtkPieChartActor
  vtkPolarAxesActor
  vtkPolarAxesActor2D
  vtkProp3DAxisFollower
  vtkRadialGridActor2D
  vtkScalarBarActor
  vtkSpiderPlotActor
  vtkXYPlotActor)

set(private_classes
vtkArcGridActorPrivate
vtkAxisGridActorPrivate)

set(nowrap_headers
  vtkScalarBarActorInternal.h)

vtk_module_add_module(VTK::RenderingAnnotation
  CLASSES ${classes}
  PRIVATE_CLASSES ${private_classes}
  NOWRAP_HEADERS ${nowrap_headers})
vtk_add_test_mangling(VTK::RenderingAnnotation)

# FIXME: vtkProp3DAxisFollower.cxx:29:14: error: redefinition of 'const double {anonymous}::AxisAlignedY [3][4][2][3]'
set_property(SOURCE vtkProp3DAxisFollower.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
