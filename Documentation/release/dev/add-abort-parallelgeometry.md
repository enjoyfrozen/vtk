# Add abort functionality to ParallelGeometry filters in VTK

ParallelGeometry filters now call `CheckAbort` to allow for
safe interruption during execution.
