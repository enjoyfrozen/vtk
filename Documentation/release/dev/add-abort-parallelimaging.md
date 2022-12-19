# Add abort functionality to ParallelImaging filters in VTK

ParallelImaging filters now call `CheckAbort` to allow for
safe interruption during execution.
