# Add abort functionality to ParallelMPI filters in VTK

ParallelMPI filters now call `CheckAbort` to allow for
safe interruption during execution.
