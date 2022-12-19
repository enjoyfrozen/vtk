# Add abort functionality to ParallelFlowPaths filters in VTK

ParallelFlowPaths filters now call `CheckAbort` to allow for
safe interruption during execution.
