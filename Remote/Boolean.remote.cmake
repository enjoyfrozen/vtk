#
# A new boolean operations filter for VTK by Ronald Romer
#

vtk_fetch_module(Boolean
  "A new boolean operation filter by Ronald Romer"
  GIT_REPOSITORY https://github.com/lorensen/vtkbool
  # 9 September - fix windows linkage
  GIT_TAG f18992b270661e00e1418b89fafb57ae339abd31
  )
