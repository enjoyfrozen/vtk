import sys
import vtk

m = vtk.vtkDICOMMetaData()

m.Set(vtk.vtkDICOMTag(0x0008, 0x0005), 'ISO_IR 100')

v = m.Get(vtk.vtkDICOMTag(0x0008, 0x0005))

if v.AsString() != 'ISO_IR 100':
    sys.exit(1)
