# Add SelectArraysExtractBlocks meta-filter

This filter combines `vtkPassSelectedArrays` and `vtkExtractBlockUsingDataAssembly`.
It is intended to be used as a pre-processing filter for certain composite writers.
It adds the possibility to choose the blocks to write, in addition to the data arrays.
