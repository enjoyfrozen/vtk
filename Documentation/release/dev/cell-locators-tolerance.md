## CellLocators: Make sure that when a tolerance parameters is available it can be used

`vtkCellLocator`, `vtkStaticCellLocator`, `vtkCellTreeLocator`, `vtkModifiedBSPTree`, `vtkOBBTree` have the following
functions that has a tolerance parameter, but it was not used in the implementation. Now it is used.

1. `InsideCellBounds`
2. `IntersectWithLine`
3. `FindCell`
4. `FindCellsAlongLine`
5. `FindCellsAlongPlane`
