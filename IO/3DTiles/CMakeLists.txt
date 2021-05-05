set(classes
  vtk3DTilesWriter)

set(private_classes
  TreeInformation)

vtk_module_add_module(VTK::IO3DTiles
  CLASSES ${classes}
  PRIVATE_CLASSES ${private_classes}
  )

if (NOT ANDROID AND
    NOT APPLE_IOS AND
    NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten" AND
    NOT VTK_OPENGL_USE_GLES)
  vtk_module_add_executable(vtkTiler
    tiler.cxx main.cxx cxxopts.hpp)
  target_link_libraries(vtkTiler
    PRIVATE
      VTK::CommonSystem
      VTK::IOGeometry
      VTK::IO3DTiles
      VTK::IOCityGML    
      VTK::CommonCore)
  vtk_module_autoinit(
    TARGETS vtkTiler
    MODULES VTK::RenderingOpenGL2)
endif ()
