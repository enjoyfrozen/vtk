## Cell Locators: Support Linear Transformation

vtkLocators can now implement the flag SupportLinearTransformation. Currently, the following cell locators implement
this flag: vtkCellLocator, vtkStaticCellLocator, vtkCellTreeLocator, vtkModifiedBSPTree. When is flag is on, after you
initially built the cell locator with the initial dataset and UseExistingSearchStructure and CacheCellBounds also on,
you can provide a new dataset and don't rebuild the locator. The requirement is that the new dataset is a linear
transformation (expressed using a rotation matrix and a translation vector) of the initial dataset. If that's not true,
the locator will be built again. The linear transformation is computed using
the [Kabsch Algorithm](https://en.wikipedia.org/wiki/Kabsch_algorithm). This functionality is particularly useful for
datasets that have timesteps and each timestep is a linear transformation of the first timestep.
