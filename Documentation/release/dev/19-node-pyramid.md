#19-node Pyramid

The purpose of this MR is to implement the vtkCell API of the 19-node-pyramid cell named `vtkBiQuadraticPyramid`.

Along with the addition of this API, several filters and tests will be updated to incorporate `vtkBiQuadraticPyramid`.

Filters:
* `vtkCellValidator`
* `vtkUnstructuredGridGeometryFilter`
* `vtkReflectionFilter`
* `vtkBoxClipDataset`

Tests:
* `contourQuadraticCells`
* `clipQuadraticCells`
* `ExtractEdgesQuadraticCells`
* `TestCellValidator`
* `TestUnstructuredGridGeometryFilter`
* `UnitTestCells`
* `quadCellConsistency`
* `quadraticIntersection`
* `quadraticEvaluation`
* `TestInterpolationDerivs`
* `TestInterpolationFunctions`
* `TestHigherOrderCell`
* `UnitTestDataSetSurfaceFilter`
* `TestGenericCell`
