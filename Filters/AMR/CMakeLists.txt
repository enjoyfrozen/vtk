set(classes
  vtkAMRCutPlane
  vtkAMRExtractLabelledBlocks
  vtkAMRGaussianPulseSource
  vtkAMRResampleFilter
  vtkAMRSliceFilter
  vtkAMRToMultiBlockFilter
  vtkImageToAMR
  vtkParallelAMRUtilities)

vtk_module_add_module(VTK::FiltersAMR
  CLASSES ${classes})
