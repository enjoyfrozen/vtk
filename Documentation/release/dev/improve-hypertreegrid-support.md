# Improve vktHyperTreeGrid support

Add support for vtkHyperTreeGrid in:
 * `vtkArrayCalculator`
 * numpy `dataset_adapter` wrapping.
 * `vtkProgrammableFilter`

HyperTreeGrid model relies on graph, thus initially the term of Vertices was used in its API.
But in the VTK world, HyperTreeGrid vertices are more like Cells.
Moreover 'Vertices' also has another meaning in VTK.
So, we reword vtkHyperTreeGrid `GetNumberOfVertices` to use `GetNumberOfCells`, for consistency.
