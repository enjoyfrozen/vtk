if (COMMAND "find_jar")
  set(_jogl_versions
    ${JOGL_EXTRA_VERSIONS} 2.3.2)

  find_jar(JOGL_LIB
    NAMES jogl-all
    VERSIONS ${_jogl_versions}
    DOC "Path to the JOGL jar")
  mark_as_advanced(JOGL_LIB)

  find_jar(JOGL_GLUE
    NAMES gluegen-rt
    VERSIONS ${_jogl_versions}
    DOC "Path to the JOGL gluegen jar")
  mark_as_advanced(JOGL_GLUE)

  unset(_jogl_versions)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JOGL
  REQUIRED_VARS JOGL_LIB JOGL_GLUE)

if (JOGL_FOUND)
  if (NOT TARGET JOGL::glue)
    add_library(JOGL::glue STATIC IMPORTED)
    set_target_properties(JOGL::glue PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES Java
      IMPORTED_LOCATION "${JOGL_GLUE}")
  endif ()
  if (NOT TARGET JOGL::JOGL)
    add_library(JOGL::JOGL STATIC IMPORTED)
    set_target_properties(JOGL::JOGL PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES Java
      IMPORTED_LOCATION "${JOGL_LIB}"
      INTERFACE_LINK_LIBRARIES "JOGL::glue")
  endif ()
endif ()
