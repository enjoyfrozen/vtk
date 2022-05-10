## Cell Locators: Support Linear Transformation

vtkAbstractCellLocators can now implement the flag SupportLinearTransformation. Currently, all cell locators except
vtkOBBTree. When is flag is on, after you initially built the cell locator with the initial dataset and
UseExistingSearchStructure and CacheCellBounds are also on, you can provide a new dataset without the locator
rebuilding. The requirement is that the new dataset is a linear transformation (expressed using a rotation matrix and a
translation vector) of the initial dataset. If that's not true, the cell locator will be built again. The linear
transformation is computed using the [Kabsch Algorithm](https://en.wikipedia.org/wiki/Kabsch_algorithm). This
functionality is particularly useful for datasets that have time-steps and each timestep is a linear transformation of
the first timestep. Finally, the aforementioned cell locators also support ShallowCopy now.
