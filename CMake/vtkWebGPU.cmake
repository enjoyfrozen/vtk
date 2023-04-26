include(vtkOpenGLOptions)

set(VTK_SUPPORTED_WEBGPU_IMPLEMENTATIONS "dawn" "wgpu-native")

set(VTK_WEBGPU_IMPLEMENTATION
    "wgpu-native"
    CACHE
      STRING
      "Which webgpu implementation to use. Options are ${VTK_SUPPORTED_WEBGPU_IMPLEMENTATIONS}"
)
set_property(CACHE VTK_WEBGPU_IMPLEMENTATION
             PROPERTY STRINGS "${VTK_SUPPORTED_WEBGPU_IMPLEMENTATIONS}")

if(NOT (VTK_WEBGPU_IMPLEMENTATION STREQUAL "dawn" OR VTK_WEBGPU_IMPLEMENTATION
                                                     STREQUAL "wgpu-native"))
  set_property(CACHE VTK_WEBGPU_IMPLEMENTATION PROPERTY VALUE "wgpu-native")
endif()

set(WEBGPU_LIBRARIES)
set(WEBGPU_INCLUDE_DIR)
if(VTK_WEBGPU_IMPLEMENTATION STREQUAL "dawn")
  vtk_module_find_package(PACKAGE Dawn)
  # Must include these source files here.
  vtk_module_sources(
    VTK::RenderingWebGPU PRIVATE "${DAWN_GEN_SRC_DIR}/dawn/webgpu_cpp.cpp"
    "${DAWN_GEN_SRC_DIR}/dawn/dawn_proc.c")
  list(APPEND WEBGPU_LIBRARIES ${DAWN_LIBRARIES})
  list(APPEND WEBGPU_INCLUDE_DIR ${DAWN_INCLUDE_DIRS})
elseif(VTK_WEBGPU_IMPLEMENTATION STREQUAL "wgpu-native")
  vtk_module_find_package(PACKAGE WGPU)
  list(APPEND WEBGPU_LIBRARIES ${WGPU_LIBRARY})
  list(APPEND WEBGPU_INCLUDE_DIR ${WGPU_INCLUDE_DIR})
endif()

if(WEBGPU_LIBRARIES AND NOT TARGET WebGPU::WebGPU)
  include(vtkDetectLibraryType)
  vtk_detect_library_type(webgpu_library_type PATH "${WEBGPU_LIBRARIES}")
  add_library(WebGPU::WebGPU "${webgpu_library_type}" IMPORTED)
  unset(webgpu_library_type)
  set_target_properties(
    WebGPU::WebGPU
    PROPERTIES IMPORTED_LOCATION "${WEBGPU_LIBRARIES}"
               IMPORTED_IMPLIB "${WEBGPU_LIBRARIES}"
               INTERFACE_INCLUDE_DIRECTORIES "${WEBGPU_INCLUDE_DIR}")
endif()
