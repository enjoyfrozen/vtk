set(headers
  DataModelHelperFunctions.h
  DataModelFactory.h
  InternalMetadataSource.h
  PredefinedDataModel.h
  SupportedDataModels.h
)

vtk_module_sources(VTK::fides PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/DataModelHelperFunctions.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/DataModelFactory.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/InternalMetadataSource.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/PredefinedDataModel.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/SupportedDataModels.cxx
  )

vtkm_add_target_information(vtkfides
    DROP_UNUSED_SYMBOLS
    DEVICE_SOURCES
      DataModelHelperFunctions.cxx
      DataModelFactory.cxx
      InternalMetadataSource.cxx
      PredefinedDataModel.cxx
      SupportedDataModels.cxx
    )

vtk_module_include(VTK::fides PRIVATE ${CMAKE_CURRENT_LIST_DIR})

install(FILES ${headers}
  DESTINATION ${_vtk_build_HEADERS_DESTINATION}/vtkfides/fides/predefined)
