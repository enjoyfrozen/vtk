#[==[.rst:
***********************
vtkModuleWrapJavaScript
***********************
#]==]


#[==[.rst:
APIs for wrapping modules for JavaScript
#]==]

#[==[.rst:
..  cmake:command:: _vtk_module_wrap_javascript_sources


  Generate sources for using a module's classes from JavaScript. |module-impl|

  This function generates the wrapped sources for a module with  It places the list of
  generated source files and classes in variables named in the second and third
  arguments, respectively.

  .. code-block:: cmake

    _vtk_module_wrap_javascript_sources(<module> <sources> <classes>)

#]==]
function (_vtk_module_wrap_javascript_sources module sources classes)
  # the library name for a VTK::ModuleName is vtkModuleName
  _vtk_module_get_module_property("${_vtk_javascript_module}"
    PROPERTY  "library_name"
    VARIABLE  _vtk_library_name)
  # The real target name for VTK::ModuleName is ModuleName
  _vtk_module_real_target(_vtk_javascript_target_name "${_vtk_javascript_module}")
  # The library name will be vtkModuleName.js
  set(_vtk_javascript_library_name "${_vtk_library_name}.js")

  file (MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${_vtk_javascript_library_name}")
  set(_vtk_javascript_args_file "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${_vtk_javascript_library_name}/${_vtk_javascript_library_name}.$<CONFIGURATION>.args")

  set(_vtk_javascript_hierarchy_depends "${module}")
  # Get private dependencies of `module`
  _vtk_module_get_module_property("${module}"
    PROPERTY  "private_depends"
    VARIABLE  _vtk_javascript_private_depends)
  list(APPEND _vtk_javascript_hierarchy_depends
    ${_vtk_javascript_private_depends})
  # Get optional dependencies of `module`
  _vtk_module_get_module_property("${module}"
    PROPERTY  "optional_depnds"
    VARIABLE  _vtk_javascript_optional_depends)
  list(APPEND _vtk_javascript_hierarchy_depends
    ${_vtk_javascript_optional_depends})
  # Appended only if those optional dependencies are declared as targets
  foreach (_vtk_javascript_optional_depend IN LISTS _vtk_javascript_optional_depends)
    if (TARGET "${_vtk_javascript_optional_depend}")
      list(APPEND _vtk_javascript_hierarchy_depends
        "${_vtk_javascript_optional_depend}")
    endif ()
  endforeach ()

  set(_vtk_javascript_command_depends)
  set(_vtk_javascript_hierarchy_files)
  foreach (_vtk_javascript_hierarchy_depend IN LISTS _vtk_javascript_hierarchy_depends)
    _vtk_module_get_module_property("${_vtk_javascript_hierarchy_depend}"
      PROPERTY "hierarchy"
      VARIABLE _vtk_javascript_hierarchy_file)
    if (_vtk_javascript_hierarchy_file)
      list(APPEND _vtk_javascript_hierarchy_files "${_vtk_javascript_hierarchy_file}")
      get_property(_vtk_javascript_is_imported
        TARGET   "${_vtk_javascript_hierarchy_depend}"
        PROPERTY "IMPORTED")
      if (_vtk_javascript_is_imported OR CMAKE_GENERATOR MATCHES "NInja")
        list(APPEND _vtk_javascript_command_depends "${_vtk_javascript_hierarchy_file}")
      else ()
        _vtk_module_get_module_property("${_vtk_javascript_hierarchy_depend}"
          PROPERTY "library_name"
          VARIABLE _vtk_javascript_hierarchy_library_name)
        if (TARGET "${_vtk_javascript_hierarchy_library_name}-hierarchy")
          list(APPEND _vtk_javascript_command_depends "${_vtk_javascript_hierarchy_library_name}-hierarchy")
        else ()
          message(FATAL_ERROR
            "The ${_vtk_javascript_hierarchy_depend} hierarchy file is attached to a non-imported target "
            "and a hierarchy target (${_vtk_javascript_hierarchy_library_name}-hierarchy) is "
            "missing.")
        endif ()
      endif ()
    endif ()
  endforeach ()

  set(_vtk_javascript_genex_allowed 1)
  if (CMAKE_VERSION VERSION_LESS "3.19")
    get_property(_vtk_javascript_target_type
      TARGET  "${_vtk_javascript_target_name}"
      PROPERTY TYPE)
    if (_vtk_javascript_target_type STREQUAL "INTERFACE_LIBRARY")
      set(_vtk_javascript_genex_allowed 0)
    endif ()
  endif ()

  set(_vtk_javascript_genex_compile_definitions "")
  set(_vtk_javascript_genex_include_directories "")
  if (_vtk_javascript_genex_allowed)
    set(_vtk_javascript_genex_compile_definitions
      "$<TARGET_PROPERTY:${_vtk_javascript_target_name},COMPILE_DEFINITIONS>")
    set(_vtk_javascript_genex_include_directories
      "$<TARGET_PROPERTY:${_vtk_javascript_target_name},INCLUDE_DIRECTORIES>")
  else ()
    if (NOT DEFINED ENV{CI})
      message(AUTHOR_WARNING
        "JavaScript wrapping is not using target-local compile definitions or "
        "include directories. This may affect generation of the JavaScript "
        "wrapper sources for the ${module} module. Use CMake 3.19+ to "
        "guarantee intended behavior.")
    endif ()
  endif ()
  file(GENERATE
    OUTPUT  "${_vtk_javascript_args_file}"
    CONTENT "$<$<BOOL:${_vtk_javascript_genex_compile_definitions}>:\n-D\'$<JOIN:${_vtk_javascript_genex_compile_definitions},\'\n-D\'>\'>\n
$<$<BOOL:${_vtk_javascript_genex_include_directories}>:\n-I\'$<JOIN:${_vtk_javascript_genex_include_directories},\'\n-I\'>\'>\n
$<$<BOOL:${_vtk_javascript_hierarchy_files}>:\n--types \'$<JOIN:${_vtk_javascript_hierarchy_files},\'\n--types \'>\'>\n")

  # Get the list of public headers from the module
  _vtk_module_get_module_property("${module}"
    PROPERTY  "headers"
    VARIABLE  _vtk_javascript_headers)
  set(_vtk_javascript_library_classes)
  set(_vtk_javascript_library_sources)
  include(vtkModuleWrapJavaScriptExclusions)

  foreach (_vtk_javascript_header IN LISTS _vtk_javascript_headers)
    # Assume the class name matches the basename of the header file. This is a VTK convention
    get_filename_component(_vtk_javascript_basename "${_vtk_javascript_header}" NAME_WE)
    if ("${_vtk_javascript_basename}" IN_LIST vtk_module_wrap_javascript_skip_headers)
      message(STATUS "Header file ${_vtk_javascript_basename} is excluded from JS wrapping")
      continue()
    endif ()
    list(APPEND _vtk_javascript_library_classes
      "${_vtk_javascript_basename}")
    set(_vtk_javascript_source_output
      "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${_vtk_javascript_library_name}/${_vtk_javascript_basename}Embinding.cxx")
    list(APPEND _vtk_javascript_library_sources
      ${_vtk_javascript_source_output})

    set(_vtk_javascript_wrap_target "VTK::WrapJavaScript")
    set(_vtk_javascript_macros_args)
    if (TARGET VTKCompileTools::WrapJavaScript)
      set(_vtk_javascript_wrap_target "VTKCompileTools::WrapJavaScript")
      if (TARGET VTKCompileTools_macros)
        list(APPEND _vtk_javascript_command_depends
          "VTKCompileTools_macros")
        list(APPEND _vtk_javascript_macros_args
          -undef
          -imacros "${_VTKCompileTools_macros_file}")
      endif ()
    endif ()

    add_custom_command(
      OUTPUT  "${_vtk_javascript_source_output}"
      COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR}
              "$<TARGET_FILE:${_vtk_javascript_wrap_target}>"
              "@${_vtk_javascript_args_file}"
              -o "${_vtk_javascript_source_output}"
              "${_vtk_javascript_header}"
              ${_vtk_javascript_macros_args}
      IMPLICIT_DEPENDS
              CXX "${_vtk_javascript_header}"
      COMMENT "Generating JavaScript wrapper sources for ${_vtk_javascript_basename}"
      DEPENDS
        "${_vtk_javascript_header}"
        "${_vtk_javascript_args_file}"
        "$<TARGET_FILE:${_vtk_javascript_wrap_target}>"
        ${_vtk_javascript_command_depends})
  endforeach ()

  set("${sources}"
    "${_vtk_javascript_library_sources}"
    PARENT_SCOPE)
  set("${classes}"
    "${_vtk_javascript_library_classes}"
    PARENT_SCOPE)
endfunction ()

#[==[.rst:

.. cmake:command:: _vtk_module_wrap_javascript_library


  Generate a JavaScript library for a set of modules. |module-impl|

  A JavaScript module library may consist of the JavaScript wrappings of multiple
  modules. This is useful for kit-based builds where the modules part of the same
  kit belong to the same JavaScript module as well.

    .. code-block:: cmake

      _vtk_module_wrap_javascript_library(<module> ...)

  The first argument is the name of the JavaScript module. The remaining arguments
  are modules to include in the JavaScript module.

  The remaining information it uses is assumed to be provided by the
  :cmake:command:`vtk_module_wrap_javascript function`.
#]==]
function (_vtk_module_wrap_javascript_library module sources classes)
  # Generate embind C++ source code
  _vtk_module_wrap_javascript_sources("${module}" _vtk_javascript_sources _vtk_javascript_classes)
  set(${sources}
    ${_vtk_javascript_sources}
    PARENT_SCOPE)
  set(${classes}
    ${_vtk_javascript_classes}
    PARENT_SCOPE)
endfunction ()


#[==[.rst:

.. cmake:command:: vtk_module_wrap_javascript

  Wrap a set of modules for use in JavaScript. |module-wrapping-java|
  
  .. code-block:: cmake
  
    vtk_module_wrap_javascript(
      MODULES <module>...
      [WRAPPED_MODULES <varname>]
    
      [UTILITY_TARGET <target>]
    
  * ``MODULES``: (Required) The list of modules to wrap.
  * ``WRAPPED_MODULES``: (Recommended) Not all modules are wrappable. This
    variable will be set to contain the list of modules which were wrapped.
  * ``UTILITY_TARGET``: If specified, all libraries made by the Java wrapping
    will link privately to this target. This may be used to add compile flags
    to the Java libraries.
  
  For each wrapped module, a ``<module>JavaScript`` target will be created. These targets
  will have a ``_vtk_module_javascript_files`` property which is the list of generated
  JavaScript, TypeScript sources and the WebAssembly binary for that target.
#]==]
function (vtk_module_wrap_javascript)
  cmake_parse_arguments(PARSE_ARGV 0 _vtk_javascript
  ""
  "WRAPPED_MODULES;UTILITY_TARGET"
  "MODULES")

  if (_vtk_javascript_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "Unparsed arguments for vtk_module_wrap_javascript: "
      "$(_vtk_javascript_UNPARSED_ARGUMENTS)")
  endif ()

  if (NOT _vtk_javascript_MODULES)
    message(WARNING
      "No modules were requested for java wrapping.")
    return ()
  endif ()

  set(_vtk_javascript_binding_sources)
  set(_vtk_javascript_binding_classes)
  set(_vtk_javascript_library_link_depends)
  include(vtkModuleWrapJavaScriptExclusions)

  foreach (_vtk_javascript_module IN LISTS _vtk_javascript_MODULES)
    # Can the module ever be wrapped?
    _vtk_module_get_module_property("${_vtk_javascript_module}"
      PROPERTY  "exclude_wrap"
      VARIABLE  _vtk_javascript_exclude_wrap)
    if (_vtk_javascript_exclude_wrap)
      continue ()
    endif ()
    # Development purpose: should the module be skipped?
    if ("${_vtk_javascript_module}" IN_LIST vtk_module_wrap_javascript_skip_modules)
      message(STATUS "Module ${_vtk_javascript_module} is excluded from JS wrapping")
      continue()
    endif ()
    # GGenerate binding source code
    _vtk_module_wrap_javascript_library("${_vtk_javascript_module}" _vtk_javascript_library_binding_sources _vtk_javascript_library_binding_classes)
    list(APPEND _vtk_javascript_binding_sources
      ${_vtk_javascript_library_binding_sources})
    list(APPEND _vtk_javascript_binding_classes
      ${_vtk_javascript_library_binding_classes})
    # Get link dependencies
    _vtk_module_get_module_property("${_vtk_javascript_module}"
    PROPERTY  "depends"
    VARIABLE  _vtk_javascript_module_depends)
    # Add the link library names to the link list
    foreach (_vtk_javascript_module_depend IN LISTS _vtk_javascript_module_depends)
      # Do not add unwrappable libraries from link list
      _vtk_module_get_module_property("${_vtk_javascript_module_depend}"
        PROPERTY  "exclude_wrap"
        VARIABLE  _vtk_javascript_module_depend_exclude_wrap)
      if (_vtk_javascript_module_depend_exclude_wrap)
        continue ()
      endif ()

      _vtk_module_get_module_property("${_vtk_javascript_module_depend}"
        PROPERTY  "library_name"
        VARIABLE  _vtk_javascript_depend_library_name)
      list(APPEND _vtk_javascript_library_link_depends
        "${_vtk_javascript_depend_library_name}")
    endforeach ()
    list(APPEND _vtk_javascript_all_wrapped_modules
      "${_vtk_javascript_module}")
  endforeach ()

  if (NOT _vtk_javascript_binding_sources)
    return ()
  endif ()

  if (_vtk_javascript_library_link_depends)
    list(REMOVE_DUPLICATES _vtk_javascript_library_link_depends)
  endif ()

  # Build vtkweb.[js, wasm]
  set(_vtk_javascript_target "vtkweb")
  add_executable("${_vtk_javascript_target}"
    ${_vtk_javascript_binding_sources})
  set_source_files_properties(${_vtk_javascript_binding_sources} PROPERTIES UNITY_GROUP vtkwebembind)
  set_target_properties("${_vtk_javascript_target}" PROPERTIES
    UNITY_BUILD TRUE
    UNITY_BUILD_MODE GROUP)
  if (_vtk_javascript_UTILITY_TARGET)
    target_link_libraries("${_vtk_javascript_target}"
      PRIVATE
        "${_vtk_javascript_UTILITY_TARGET}")
  endif ()

  # TODO: populate with generated js, wasm and ts file(s)
  # set_target_properties("${_vtk_javascript_target}"
  #   PROPERTIES "_vtk_module_javascript_files" "${_vtk_javascript_library_sources}")
  
  vtk_module_autoinit(
    MODULES ${_vtk_javascript_MODULES}
    TARGETS "${_vtk_javascript_target}")

  target_link_libraries("${_vtk_javascript_target}"
    PRIVATE
      VTK::WrappingJavaScript
      ${_vtk_javascript_MODULES})

  list(APPEND emscripten_link_options
    "-lembind"
    "-sWASM=1"
    "-sMODULARIZE=1"
    "-sEXPORT_ES6=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sEXPORT_NAME=VTKWebAssemblyModule"
    "-sEXPORTED_RUNTIME_METHODS=['ENV', 'FS', 'ccall', 'stringToNewUTF8', 'addFunction']"
    "-sEXPORTED_FUNCTIONS=['_free', '_malloc']"
    # "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=['$addFunction']" # CMake adds another $ sign!
    "-sINCLUDE_FULL_LIBRARY" # for addFunction
    "-sALLOW_TABLE_GROWTH=1"
    "-sERROR_ON_UNDEFINED_SYMBOLS=0"
  )

  # TODO: Move these to vtkCompilerPlatformFlags? Options must be applied on the vtk c++ translation units too.
  set(emscripten_debug_options)
  set(emscripten_optimizations)

  if (VTK_WASM_ARCHITECTURE STREQUAL "WASM32")
    list(APPEND emscripten_compile_options "-sMEMORY64=0")
    list(APPEND emscripten_link_options "-sMEMORY64=0")
  elseif (VTK_WASM_ARCHITECTURE STREQUAL "WASM64")
    list(APPEND emscripten_compile_options "-sMEMORY64=1")
    list(APPEND emscripten_link_options "-sMEMORY64=1")
    list(APPEND emscripten_link_options "-sWASM_BIGINT=1")
  elseif (VTK_WASM_ARCHITECTURE STREQUAL "WASM32_64")
    list(APPEND emscripten_compile_options "-sMEMORY64=2")
    list(APPEND emscripten_link_options "-sMEMORY64=2")
    list(APPEND emscripten_link_options "-sWASM_BIGINT=1")
  endif()

  if(VTK_WASM_DEBUGINFO STREQUAL "NONE")
    list(APPEND emscripten_debug_options
      "-g0")
  elseif(VTK_WASM_DEBUGINFO STREQUAL "READABLE_JS")
    list(APPEND emscripten_debug_options
      "-g1")
    list(APPEND emscripten_link_options
      "-sDEMANGLE_SUPPORT=1")
  elseif(VTK_WASM_DEBUGINFO STREQUAL "PROFILE")
    list(APPEND emscripten_debug_options
      "-g2")
    list(APPEND emscripten_link_options
      "-sDEMANGLE_SUPPORT=1")
  elseif(VTK_WASM_DEBUGINFO STREQUAL "DEBUG_NATIVE")
    list(APPEND emscripten_debug_options
      "-g3")
    list(APPEND emscripten_link_options
      "-sASSERTIONS=1"
      "-sDEMANGLE_SUPPORT=1")
  endif()

  if(VTK_WASM_OPTIMIZATION STREQUAL "NO_OPTIMIZATION")
    list(APPEND emscripten_optimizations
      "-O0")
  elseif(VTK_WASM_OPTIMIZATION STREQUAL "LITTLE")
    list(APPEND emscripten_optimizations
      "-O1")
  elseif(VTK_WASM_OPTIMIZATION STREQUAL "MORE")
    list(APPEND emscripten_optimizations
      "-O2")
  elseif(VTK_WASM_OPTIMIZATION STREQUAL "BEST")
    list(APPEND emscripten_optimizations
      "-O3")
  elseif(VTK_WASM_OPTIMIZATION STREQUAL "SMALL")
    list(APPEND emscripten_optimizations
      "-Os")
  elseif(VTK_WASM_OPTIMIZATION STREQUAL "SMALLEST")
    list(APPEND emscripten_optimizations
      "-Oz")
  elseif(VTK_WASM_OPTIMIZATION STREQUAL "SMALLEST_WITH_CLOSURE")
    list(APPEND emscripten_optimizations
      "-Oz")
    list(APPEND emscripten_link_options
      "--closure 1")
  endif()

  target_compile_options("${_vtk_javascript_target}"
    PUBLIC
      ${emscripten_compile_options}
      ${emscripten_optimizations}
      ${emscripten_debug_options})

  target_link_options("${_vtk_javascript_target}"
    PUBLIC
      ${emscripten_link_options}
      ${emscripten_optimizations}
      ${emscripten_debug_options})

  if (NOT _vtk_javascript_all_wrapped_modules)
    message(FATAL_ERROR
      "None of the given modules could be wrapped.")
  endif ()

  if (DEFINED _vtk_javascript_WRAPPED_MODULES)
    set("${_vtk_javascript_WRAPPED_MODULES}"
      "${_vtk_javascript_all_wrapped_modules}"
      PARENT_SCOPE)
  endif ()
endfunction()
