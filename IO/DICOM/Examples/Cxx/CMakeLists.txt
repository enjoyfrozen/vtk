# These examples are prefixed with "Test" because they were written as
# tests that had to be run by hand.  Eventually, test data will be added
# and these will be automated.  For now, they serve as useful examples of
# how DICOM can be used from VTK.

set(examples
  TestDICOMCompiler
  TestDICOMDirectory
  TestDICOMFileSorter
  TestDICOMReader
  TestDICOMWriter
  TestDICOMRealWorldValue
)

set(display_examples
  TestDICOMDisplay
  TestDICOMOverlay
)

foreach(example ${examples})
  vtk_module_add_executable(${example} ${example}.cxx)
  target_link_libraries(${example} VTK::IODICOM)
endforeach()

if (TARGET VTK::RenderingImage AND
    TARGET VTK::RenderingOpenGL2 AND
    TARGET VTK::InteractionStyle)
  foreach(example ${display_examples})
    vtk_module_add_executable(${example} ${example}.cxx)
    target_link_libraries(${example}
      PRIVATE VTK::IODICOM
        VTK::ImagingCore
        VTK::RenderingCore
        VTK::RenderingImage
        VTK::RenderingOpenGL2
        VTK::InteractionStyle)
  endforeach()
  vtk_module_autoinit(TARGETS ${display_examples}
      MODULES VTK::RenderingOpenGL2 VTK::InteractionStyle)
endif ()
