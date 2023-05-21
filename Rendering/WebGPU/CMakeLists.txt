set(classes
  # vtkWebGPUActor
  # vtkWebGPUCamera
  # vtkWebGPUClearPass
  # vtkWebGPUHardwareSelector
  # vtkWebGPULight
  # vtkWebGPUPolyDataMapper
  # vtkWebGPUProperty
  # vtkWebGPURenderPass
  # vtkWebGPURenderer
  vtkWebGPUDevice
  vtkWebGPUForwardPass
  vtkWebGPUInstance
  vtkWebGPUObject
  vtkWebGPUOpaquePass
  vtkWebGPURenderPassEncoder
  vtkWebGPURenderWindow
  vtkWebGPURendererNode
  vtkWebGPUTexture
  vtkWebGPUTextureView
  vtkWebGPUViewNode
  vtkWebGPUViewNodeFactory
  vtkWebGPUWindowNode
)

set(private_classes
  # vtkWebGPUInternalsBindGroup
  # vtkWebGPUInternalsBindGroupLayout
  # vtkWebGPUInternalsBuffer
  # vtkWebGPUInternalsPipelineLayout
  # vtkWebGPUInternalsRenderPassCreateInfo
  # vtkWebGPUInternalsRenderPassDescriptor
  # vtkWebGPUInternalsRenderPipelineDescriptor
  # vtkWebGPUInternalsShaderModule
)

unset(wgsl_shader_sources)
unset(wgsl_shader_headers)
set(shader_files
  wgsl/PolyData.wgsl
)
foreach (file IN LISTS shader_files)
  vtk_encode_string(
    INPUT         "${file}"
    EXPORT_SYMBOL "VTKRENDERINGWEBGPU_NO_EXPORT"
    EXPORT_HEADER "vtkRenderingWebGPUModule.h"
    HEADER_OUTPUT header
    SOURCE_OUTPUT source)
  list(APPEND wgsl_shader_sources
    "${source}")
  list(APPEND wgsl_shader_headers
    "${header}")
endforeach ()

vtk_object_factory_declare(
  BASE "vtkRenderWindow"
  OVERRIDE "vtkWebGPURenderWindow"
)

vtk_object_factory_configure(
  SOURCE_FILE vtk_object_factory_source
  HEADER_FILE vtk_object_factory_header
  EXPORT_MACRO "VTKRENDERINGWEBGPU_EXPORT"
  EXTRA_INCLUDES "<vtkCollection.h>" "<vtkObjectFactoryCollection.h>" "<vtkLogger.h>" "<cstdlib>"
  INITIAL_CODE_FILE "vtkWebGPUObjectFactoryInit.cxx")

list(APPEND vtk_wgpu_private_headers
)
list(APPEND vtk_wgpu_public_headers
  vtk_wgpu.h
)
list(APPEND vtk_wgpu_priv_sources
)

vtk_module_add_module(VTK::RenderingWebGPU
  CLASSES ${classes}
  PRIVATE_CLASSES ${private_classes}
  SOURCES ${vtk_object_factory_source} ${vtk_wgpu_priv_sources} ${wgsl_shader_sources}
  HEADERS ${vtk_wgpu_public_headers}
  PRIVATE_HEADERS ${vtk_object_factory_header} ${vtk_wgpu_private_headers} ${wgsl_shader_headers}
)

vtk_module_compile_features(VTK::RenderingWebGPU
  PUBLIC
    cxx_std_14)

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  vtk_module_include(VTK::RenderingWebGPU
    INTERFACE
      "${EMSCRIPTEN_ROOT_PATH}/cache/sysroot/include/webgpu"
  )
  vtk_module_link_options(VTK::RenderingWebGPU
    INTERFACE
      "SHELL:-s USE_WEBGPU=1"
  )
  vtk_module_find_package(PACKAGE SDL2)
  vtk_module_link(VTK::RenderingWebGPU
    PUBLIC
      SDL2::SDL2)
else ()
  # WebGPU native
  include(vtkWebGPU)
  vtk_module_link(VTK::RenderingWebGPU
    PUBLIC WebGPU::WebGPU)
  set(WGPU_CMPDEF)
  if (VTK_WEBGPU_IMPLEMENTATION STREQUAL "dawn")
    set(WGPU_CMPDEF "VTK_WEBGPU_USE_DAWN")
  else ()
    set(WGPU_CMPDEF "VTK_WEBGPU_USE_WGPU")
  endif ()
  vtk_module_definitions(VTK::RenderingWebGPU PUBLIC ${WGPU_CMPDEF})
endif ()
