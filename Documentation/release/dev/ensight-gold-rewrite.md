## Rewrite reader for EnSight Gold files

The EnSight Gold reader is in the process of being rewritten.
vtkNewEnSightGoldReader doesn't use the complex class hierarchy used with the previous EnSight readers, which should be easier to maintain/debug.
This reader handles both ASCII and binary files with an internal class handling the appropriate way to handle reading the different types of files, instead of having separate VTK classes to handle ASCII and binary.
In addition, the new reader outputs a PartitionedDataSetCollection instead of Multiblock.

EnSight Gold Features currently implemented:
- Basic case file parsing
- Geometry files: can parse uniform, rectilinear, and curvilinear parts, with all possible options (e.g., blanked nodes, ghost cells, etc)
- ASCII and C binary both supported

To be implemented:
- Fortran binary
- Unstructured grid
- Variable files
- time set/file set
- undefined and partial variable values
- measured geometry files
- Server-of-server casefile support
