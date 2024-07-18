# TODO: Update FindJNI to use imported targets.
find_package(JNI REQUIRED)

set(classes
  vtkJavaUtil)

set(headers
  vtkJavaAwt.h)

vtk_module_add_module(VTK::Java
  CLASSES ${classes}
  HEADERS ${headers}
)

foreach(jni_include_dir IN LISTS JNI_INCLUDE_DIRS)
  # We cannot pass a list to vtk_module_include inside a BUILD_INTERFACE
  vtk_module_include(VTK::Java SYSTEM PUBLIC
    $<BUILD_INTERFACE:${jni_include_dir}>
  )
endforeach()

vtk_module_link(VTK::Java
  PUBLIC
    ${JNI_LIBRARIES}
)
