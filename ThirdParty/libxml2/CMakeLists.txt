include(CMakeDependentOption)

cmake_dependent_option(VTK_MODULE_vtklibxml2_IS_SHARED "Whether the system libxml2 in use is a shared library or not" ON
  VTK_USE_SYSTEM_LIBXML2 "${BUILD_SHARED_LIBS}")

if (NOT VTK_MODULE_vtklibxml2_IS_SHARED)
  set(XML_STATIC 1)
endif()

vtk_module_third_party(LibXml2
  INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/vtklibxml2
)
