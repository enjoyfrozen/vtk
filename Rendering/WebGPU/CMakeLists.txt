set(classes
  # vtkWebGPUActor
  # vtkWebGPUCamera
  # vtkWebGPUClearPass
  # vtkWebGPUHardwareSelector
  # vtkWebGPULight
  vtkWebGPURenderWindow
  # vtkWebGPUPolyDataMapper
  # vtkWebGPUProperty
  # vtkWebGPURenderer
  # vtkWebGPURenderPass
  vtkWebGPUInstance
  vtkWebGPUDevice
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

# setup factory overrides
# NI - Not Implemented
set(webgpu_overrides
  #  Actor
  #  # NI: BillboardTextActor3D
  #  Camera
  #  # NI: LabeledContourMapper
  #  HardwareSelector
  #  # NI: ImageMapper
  #  # NI: ImageSliceMapper
  #  # NI: Glyph3DMapper
  #  # NI: HyperTreeGridMapper
  #  Light
  #  # NI: PointGaussianMapper
  #  PolyDataMapper
  #  # NI: PolyDataMapper2D
  #  Property
  #  # NI: ShaderProperty
  #  # NI: Uniforms
  #  Renderer
  # NI: RenderTimerLog
  # NI: Skybox
  # NI: TextActor
  # NI: TextActor3D
  # NI: TextMapper
  # NI: Texture
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

foreach (webgpu_override IN LISTS webgpu_overrides)
  vtk_object_factory_declare(
    BASE "vtk${webgpu_override}"
    OVERRIDE "vtkWebGPU${webgpu_override}")
endforeach ()

set(has_vtkRenderWindow_override 0)
# Setup factory overrides and add window system specific render windows.
if (VTK_USE_X)
  # Add some custom overrides
  if (NOT VTK_DEFAULT_RENDER_WINDOW_HEADLESS)
    vtk_object_factory_declare(
      BASE vtkRenderWindow
      OVERRIDE vtkXWebGPURenderWindow)
    set(has_vtkRenderWindow_override 1)
  endif ()
  list(APPEND classes vtkXWebGPURenderWindow)
elseif (VTK_USE_WIN32_OPENGL)
  # Add some custom overrides
  if (NOT VTK_DEFAULT_RENDER_WINDOW_HEADLESS)
    vtk_object_factory_declare(
      BASE vtkRenderWindow
      OVERRIDE vtkWin32WebGPURenderWindow)
    set(has_vtkRenderWindow_override 1)
  endif ()
  list(APPEND classes
    vtkWin32WebGPURenderWindow)
elseif (VTK_USE_COCOA)
  # Add some custom overrides
  if (NOT VTK_DEFAULT_RENDER_WINDOW_HEADLESS)
    vtk_object_factory_declare(
      BASE vtkRenderWindow
      OVERRIDE vtkCocoaWebGPURenderWindow)
    set(has_vtkRenderWindow_override 1)
  endif ()
  list(APPEND sources
    vtkCocoaWebGPURenderWindow.mm
    vtkCocoaWebGPUView.mm)
  list(APPEND headers
    vtkCocoaWebGPURenderWindow.h)
  list(APPEND nowrap_headers
    vtkCocoaWebGPUView.h)
  # Set specified Objective-C++ flags, if any.
  if (VTK_REQUIRED_OBJCXX_FLAGS)
    set_source_files_properties(
      vtkCocoaWebGPURenderWindow.mm
      vtkCocoaWebGPUView.mm
      PROPERTIES
        COMPILE_FLAGS "${VTK_REQUIRED_OBJCXX_FLAGS}")
  endif ()
elseif (EMSCRIPTEN)
  # Add some custom overrides
  vtk_object_factory_declare(
    BASE vtkRenderWindow
    OVERRIDE vtkSDL2WebGPURenderWindow)
  set(has_vtkRenderWindow_override 1)
endif ()

if (VTK_USE_SDL2)
  list(APPEND classes vtkSDL2WebGPURenderWindow)
endif ()

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
  # vtkWGPUContext.h
)
list(APPEND vtk_wgpu_priv_sources
  # vtkWGPUContext.cxx
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

if (VTK_USE_X)
  vtk_module_find_package(PACKAGE X11)
  vtk_module_link(VTK::RenderingWebGPU PUBLIC X11::X11)
  if (TARGET X11::Xcursor)
    vtk_module_link(VTK::RenderingWebGPU PRIVATE X11::Xcursor)
  else()
    message(WARNING "X11::Xcursor not found; custom cursors will be ignored.")
  endif()
  set_property(SOURCE vtkXWebGPURenderWindow.cxx APPEND
    PROPERTY
      COMPILE_DEFINITIONS "VTK_HAVE_XCURSOR=$<TARGET_EXISTS:X11::Xcursor>"
  )
elseif (VTK_USE_COCOA)
  vtk_module_link(VTK::RenderingWebGPU PUBLIC "-framework Cocoa")
elseif (VTK_USE_WIN32_OPENGL)
  vtk_module_find_package(PACKAGE DirectX)
  vtk_module_link(VTK::RenderingWebGPU PRIVATE DirectX::d3d11 DirectX::dxgi)
endif ()
