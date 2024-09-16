set(classes
  vtkRadiossAnimReader
  RadiossAnimDataModel
  RadiossAnimFile)

vtk_module_add_module(VTK::IORadioss
  CLASSES ${classes}
  )

vtk_add_test_mangling(VTK::IORadioss)
