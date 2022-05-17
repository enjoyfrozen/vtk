## Add enable/disable to vtkExtractBlockUsingDataAssembly

Add a setter/getter to `vtkExtractBlockUsingDataAssembly` in order to enable or diable it.
When disabled, it simply acts as a pass-through filter.
Introduced alongside the `vtkSelectArraysExtractBlocks` meta-filter which use the `vtkExtractBlockUsingDataAssembly` filter in its internal pipeline.
