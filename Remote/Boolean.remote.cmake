#
# A new boolean operations filter for VTK by Ronald Romer
#

vtk_fetch_module(Boolean
  "A new boolean operation filter by Ronald Romer"
  GIT_REPOSITORY https://github.com/lorensen/vtkbool
  # 01 September - fix windows linker errors
  GIT_TAG d590b3250c7643c3a850f5111d65244ba525eac1
  )
