include("${CMAKE_CURRENT_LIST_DIR}/configure_windows.cmake")

set(VTK_JAVA_SOURCE_VERSION $ENV{VTK_JAVA_VERSION} CACHE STRING "" FORCE)
set(VTK_JAVA_TARGET_VERSION $ENV{VTK_JAVA_VERSION} CACHE STRING "" FORCE)

set(JOGL_GLUE "$ENV{HOMEPATH}/.m2/repository/org/jogamp/gluegen/gluegen-rt/2.3.2/gluegen-rt-2.3.2.jar" CACHE FILEPATH "")
set(JOGL_LIB  "$ENV{HOMEPATH}/.m2/repository/org/jogamp/jogl/jogl-all/2.3.2/jogl-all-2.3.2.jar" CACHE FILEPATH "")
