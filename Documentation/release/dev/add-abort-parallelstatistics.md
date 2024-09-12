# Add abort functionality to ParallelStatistics filters in VTK

ParallelMPI filters now call `CheckAbort` to allow for
safe interruption during execution.
