# Add abort functionality to Parallel filters in VTK

Parallel filters now call `CheckAbort` to allow for
safe interruption during execution.
