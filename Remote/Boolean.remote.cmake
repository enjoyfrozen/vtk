#
# A new boolean operations filter for VTK by Ronald Romer
#

vtk_fetch_module(Boolean
  "A new boolean operation filter by Ronald Romer"
  GIT_REPOSITORY https://github.com/lorensen/vtkbool
  # 12 September - fix unit test
  GIT_TAG f5cf5714c7981e83865d55a2480d8938c28e901e
  )
