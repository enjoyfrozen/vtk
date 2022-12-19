# Add abort functionality to ParallelDIY2 filters in VTK

ParallelDIY2 filters now call `CheckAbort` to allow for
safe interruption during execution.
