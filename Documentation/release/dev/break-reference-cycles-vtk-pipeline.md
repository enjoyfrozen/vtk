Break Reference Cycles In VTK Pipeline

- The VTK pipeline classes have been slightly refactored to avoid the
necessity of a garbage collector.
- The consumer always hard references its producers. This is done with
`vtkAlgorithm::SetInputConnection()` or `vtkAlgorithm::GetInputConnection()`.
- The `vtkExecutive` no longer manages the lifetime of its `vtkAlgorithm` instance.
- The `vtkExecutive::PRODUCER()` and `vtkExecutive::CONSUMER()` keys were primarily
used to store executives and their port values. These information keys are deprecated
since both of them introduced complex circular references.
