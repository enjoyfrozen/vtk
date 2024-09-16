cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

set(MYNAME QtQuickCone)

project(${MYNAME})

find_package(VTK COMPONENTS 
  vtkCommonColor
  vtkCommonCore
  vtkCommonDataModel
  vtkFiltersSources
  vtkInteractionStyle
  vtkRenderingContextOpenGL2
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2
  vtkGUISupportQtQuick2
  QUIET
)

if (NOT VTK_FOUND)
  message("Skipping ${MYNAME}: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()

set(QT_DIR ${Qt5_DIR})

find_package(QT NAMES Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

set(PROJECT_SOURCES
  main.cpp
  main.qml
  qml.qrc
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(${MYNAME} MACOSX_BUNDLE
  ${PROJECT_SOURCES}
)

get_filename_component(QT_ROOT "../../.." 
  REALPATH BASE_DIR "${Qt5_DIR}"
)

message (STATUS "QT_VERSION: Qt${QT_VERSION_MAJOR}")
message (STATUS "QT_ROOT: ${QT_ROOT}")
message (STATUS "Qt5_DIR: ${Qt5_DIR}")
message (STATUS "VTK_VERSION: ${VTK_VERSION}")
message (STATUS "VTK_DIR: ${VTK_DIR}")

if (VTK_VERSION VERSION_LESS "8.90.0")
  # old system
  include(${VTK_USE_FILE})
  target_link_libraries(${MYNAME} PRIVATE ${VTK_LIBRARIES})
else ()
  # include all components
  target_link_libraries(${MYNAME} PRIVATE ${VTK_LIBRARIES})
  # vtk_module_autoinit is needed
  vtk_module_autoinit(
    TARGETS ${MYNAME}
    MODULES ${VTK_LIBRARIES}
    )
endif ()

target_compile_definitions(${MYNAME} PRIVATE 
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)
target_link_libraries(${MYNAME} PRIVATE 
  Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick
)
set_target_properties(${MYNAME} PROPERTIES 
  VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${VTK_DIR}/Bin/Debug;${QT_ROOT}/bin;${QT_ROOT}/plugins/platforms"
)
set_target_properties(${MYNAME} PROPERTIES 
  LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
  LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY 
  VS_STARTUP_PROJECT ${MYNAME}
)
