cmake_minimum_required(VERSION 3.14)

set(MYNAME QtQuickCone)

project(${MYNAME} VERSION 0.1 LANGUAGES CXX)

set(PROJECT_SOURCES
  main.cpp
  main.qml
  qml.qrc
)

include(../Common.cmake)
