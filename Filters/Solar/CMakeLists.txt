set(classes
  vtkEmbreePointLocator)

set(nowrap_classes)

set(nowrap_headers)

vtk_module_find_package(
    PACKAGE embree
    VERSION 3.12)

vtk_module_add_module(VTK::FiltersSolar
  CLASSES ${classes})

vtk_module_link(VTK::FiltersSolar
  PUBLIC
    embree
  )
