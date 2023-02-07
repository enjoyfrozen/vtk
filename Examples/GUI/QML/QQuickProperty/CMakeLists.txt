cmake_minimum_required(VERSION 3.14)

set(MYNAME QQuickProperty)

project(${MYNAME} VERSION 1.0)

set(PROJECT_SOURCES
  main.cpp
  main.qml
  DynamicSplitView.qml
  qml.qrc
)

include(../Common.cmake)
