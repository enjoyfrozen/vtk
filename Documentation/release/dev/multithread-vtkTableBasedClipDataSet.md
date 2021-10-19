## Multithread vtkTableBasedClipDataSet

`vtkTableBasedClipDataSet` has been multithreaded using `vtkSMPTools`.

Additionally, design a fast thread-safe call of `void GetCellPoints(vtkIdType cellId, vtkIdType& npts, vtkIdType const*& pts, vtkIdList* ptIds)` for `vtkPolyData` and `vtkUnstructuredGrid`.
