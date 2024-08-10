set(xgc_headers
  XGCCommon.h
)

vtk_module_sources(VTK::fides PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/XGCCommon.cxx)

vtkm_add_target_information(vtkfides
    DROP_UNUSED_SYMBOLS
    DEVICE_SOURCES XGCCommon.cxx)

vtk_module_include(VTK::fides PRIVATE ${CMAKE_CURRENT_LIST_DIR})

install(FILES ${xgc_headers}
  DESTINATION ${_vtk_build_HEADERS_DESTINATION}/vtkfides/fides/xgc)
