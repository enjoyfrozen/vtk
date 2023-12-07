## Introduce vtkADIOS2DataArrayWriter


VTK now provides vtkADIOS2DataArrayWriter that can be used to save
vtkDataArrays to an ADIOS2 BP file.  vtkDataArrays are added as adios variables
of the appropriate type. Additionally, you can add string attributes to the file
for holding metadata.
