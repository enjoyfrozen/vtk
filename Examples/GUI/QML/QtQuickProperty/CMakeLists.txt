cmake_minimum_required(VERSION 3.14)

set(MYNAME QtQuickProperty)

project(${MYNAME} VERSION 1.0)

set(PROJECT_SOURCES
  main.cpp
  main.qml
  DynamicSplitView.qml
  qml.qrc
)

find_package(VTK COMPONENTS CommonCore CommonColor GUISupportQtQuick) 

if (NOT VTK_FOUND)
  message("Skipping ${MYNAME}: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()

include(../Common.cmake)

target_link_libraries(${MYNAME} PRIVATE ${VTK_LIBRARIES})
vtk_module_autoinit( TARGETS ${MYNAME} MODULES ${VTK_LIBRARIES} )

message (STATUS "VTK_DIR: ${VTK_DIR}")
message (STATUS "VTK_VERSION: ${VTK_VERSION}")