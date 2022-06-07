## Removing array naming behavior introduce in vtk 9.1.0 for the vtkTableFFT

The vtkTableFFT filter is now consistent regarding the naming of tables
processed by an FFT between the input and the output, they are no longer
prefixed with FFT_ exactly like the behavior of this filter in vtk 9.0.0.
