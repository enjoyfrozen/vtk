set(VTK_BUILD_TESTING "OFF"
  CACHE STRING "Build module testing directories")
set_property(CACHE VTK_BUILD_TESTING
  PROPERTY
    STRINGS "ON;OFF;WANT")

if (VTK_BUILD_TESTING)
  if (NOT VTK_DATA_STORE)
    # These checks must be synchronized with vtkExternalData.cmake
    if (NOT EXISTS "${VTK_SOURCE_DIR}/.ExternalData/README.rst" AND
        NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/../VTKExternalData" AND
        NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/../ExternalData" AND
        NOT DEFINED "ENV{VTKExternalData_OBJECT_STORES}" AND
        NOT DEFINED "ENV{ExternalData_OBJECT_STORES}")

      # The file .ExternalData/README.rst exists in the VTK git repository
      # but is not included in the VTK release tarballs.  For them, the data
      # directory is expected to exist outside of the source tree.
      message(FATAL_ERROR "VTK_BUILD_TESTING is ${VTK_BUILD_TESTING}, but no "
              "external data directory has been specified! Please create a "
              "directory called \"ExternalData\" or \"VTKExternalData\" at "
              "the same level as the VTK source tree, or set VTK_DATA_STORE "
              "to the name of the desired data directory.")
    endif ()
  endif ()

  include(vtkExternalData)
  include(CTest)
  set_property(CACHE BUILD_TESTING
    PROPERTY
      TYPE INTERNAL)
  set(BUILD_TESTING ON)
else ()
  set(BUILD_TESTING OFF)
endif ()

# Provide an option for tests requiring "large" input data
option(VTK_USE_LARGE_DATA "Enable tests requiring \"large\" data" OFF)
