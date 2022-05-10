## Add vtkLinearTransformCellLocator

vtkLinearTransformCellLocator is class a cell locator adaptor that can accept any cell locator, e.g.
vtkStaticCellLocator or vtkCellLocator, calculate the transformation matrix of the original dataset versus the new
inside BuildLocator, and then use the cell locator to perform cell locator operations. The transformation matrix is
computed using the https://en.wikipedia.org/wiki/Kabsch_algorithm, and IsLinearTransformation validates if the dataset
is a linear transformation of the original. This functionality is particularly useful for datasets that have time-steps
and each timestep is a linear transformation of the first timestep. Finally, the vtkCellLocator, vtkStaticCellLocator,
vtkCellTreeLocator, vtkModifiedBSPTree, and vtkLinearTransformCellLocator cell locators also support ShallowCopy now.
