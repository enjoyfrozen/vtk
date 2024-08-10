set(sources
  src/glew.c)

set(headers
  include/GL/glew.h
  include/GL/glxew.h
  include/GL/vtk_glew_mangle.h
  include/GL/wglew.h)

vtk_module_add_module(VTK::glew
  HEADER_DIRECTORIES
  SOURCES ${sources}
  HEADERS ${headers}
  HEADERS_SUBDIR "vtkglew")
vtk_module_definitions(VTK::glew
  PRIVATE
    GLEW_NO_GLU)


if(VTK_OPENGL_HAS_OSMESA)
  vtk_module_definitions(VTK::glew
    PRIVATE
      GLEW_OSMESA)
  vtk_module_link(VTK::glew
    PRIVATE
      ${CMAKE_DL_LIBS})
endif()

if(VTK_OPENGL_HAS_EGL)
  vtk_module_definitions(VTK::glew
    PRIVATE
      GLEW_EGL)
endif()

if (BUILD_SHARED_LIBS)
  if (WIN32)
    vtk_module_definitions(VTK::glew
      PRIVATE
        GLEW_BUILD)
    if(MINGW)
      # https://github.com/nigels-com/glew/issues/157
      vtk_module_link(VTK::glew
        PRIVATE
          -nostdlib)
      # https://discourse.vtk.org/t/1898
      vtk_module_compile_options(VTK::glew
        PRIVATE
          -minline-all-stringops)
    endif()
  endif ()
else ()
  vtk_module_definitions(VTK::glew
    PUBLIC
      GLEW_STATIC)
endif ()

vtk_module_include(VTK::glew
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include")
