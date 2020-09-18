
set(classes
  vtkTDXDevice
  vtkTDXinteractorStyleGeo)

if (WIN32)
  list(APPEND classes vtkTDxWinDevice)
endif ()

if (APPLE)
  list(APPEND classes vtkTDxMacDevice)
endif ()

if (VTK_USE_X AND VTK_USE_TDX)
  list(APPEND classes vtkTDxUnixDevice)
endif ()
  
vtk_module_add_module(VTK::RenderingTDx
  CLASSES ${classes}
)

target_compile_definitions(RenderingTDx PUBLIC -DVTK_USE_TDX)
if (WIN32)
  target_compile_options(RenderingTDx PRIVATE /MP1)
endif()