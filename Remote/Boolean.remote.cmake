#
# A new boolean operations filter for VTK by Ronald Romer
#

vtk_fetch_module(Boolean
  "A new boolean operation filter by Ronald Romer"
  GIT_REPOSITORY https://github.com/lorensen/vtkbool
  # 9 September - fix windows linkage
  GIT_TAG e74ec80b6074d273bfbc6a481011bcd4a4afe0ce
  )
