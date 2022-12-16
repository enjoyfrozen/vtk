find_package("Qt${vtk_qt_major_version}"
  COMPONENTS Quick Qml
  REQUIRED
  )

set(CMAKE_AUTORCC ON)

vtk_add_test_cxx(vtkGUISupportQtQuickCxxTests tests
  TestQQuickVtkItem_1.cxx
  TestQQuickVtkItem_2.cxx
  TestQQuickVtkItem_3.cxx
  TestQQuickVTKRenderItem.cxx
  TestQQuickVTKRenderItemWidget.cxx
  TestQQuickVTKRenderWindow.cxx
  )

 set(qml_files
  TestQQuickVTKRenderItem.qml
  TestQQuickVTKRenderItemWidget.qml
  TestQQuickVTKRenderWindow.qml)

foreach (qml_file IN LISTS qml_files)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${qml_file}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${qml_file}"
    @ONLY)
endforeach ()

set(qml_files2
  TestQQuickVtkItem_1.qml
  TestQQuickVtkItem_2.qml
  TestQQuickVtkItem_3.qml)

foreach (qml_file2 IN LISTS qml_files2)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${qml_file2}"
    "${CMAKE_CURRENT_BINARY_DIR}/${qml_file2}"
    COPYONLY)
endforeach ()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/TestQQuickVTK.qrc"
  "${CMAKE_CURRENT_BINARY_DIR}/TestQQuickVTK.qrc"
  COPYONLY)

vtk_test_cxx_executable(vtkGUISupportQtQuickCxxTests tests
  "${CMAKE_CURRENT_BINARY_DIR}/TestQQuickVTK.qrc"
  )

target_link_libraries(vtkGUISupportQtQuickCxxTests
  PRIVATE
    "Qt${vtk_qt_major_version}::Quick"
    "Qt${vtk_qt_major_version}::Qml"
    )

target_compile_definitions(vtkGUISupportQtQuickCxxTests PRIVATE QT_NO_KEYWORDS)
set_property(
  TEST
    VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderItem
    VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderItemWidget
    VTK::GUISupportQtQuickCxx-TestQQuickVTKRenderWindow
  APPEND
  PROPERTY
    ENVIRONMENT "QML2_IMPORT_PATH=${VTK_QML_DIR}")
