set(classes
  vtkCriticalTime
  vtkForceStaticMesh
  vtkDataObjectMeshCache
  vtkTemporalSmoothing)

vtk_module_add_module(VTK::FiltersTemporal
  CLASSES ${classes})

set_property(TARGET FiltersTemporal PROPERTY UNITY_BUILD OFF)

vtk_add_test_mangling(VTK::FiltersTemporal)
