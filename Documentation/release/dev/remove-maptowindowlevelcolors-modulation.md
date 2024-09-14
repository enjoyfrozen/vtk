# Remove post-mapping modulation from vtkImageMapToWindowLevelColors

After applying the Window/Level and mapping the result through the lookup table,
vtkImageMapToWindowLevelColors would "modulate" the resulting colors by again
multiplying them by the factor used in the Window/Level operation.  This extra
modulation, which never made any sense, has been removed.  Now we simply apply
the Window and Level, and then map through the lookup table.
