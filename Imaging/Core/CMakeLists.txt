set(classes
  vtkAbstractImageInterpolator
  vtkExtractVOI
  vtkGenericImageInterpolator
  vtkImageAppendComponents
  vtkImageBlend
  vtkImageBSplineCoefficients
  vtkImageBSplineInternals
  vtkImageBSplineInterpolator
  vtkImageCacheFilter
  vtkImageCast
  vtkImageChangeInformation
  vtkImageClip
  vtkImageConstantPad
  vtkImageDataStreamer
  vtkImageDecomposeFilter
  vtkImageDifference
  vtkImageExtractComponents
  vtkImageFlip
  vtkImageInterpolator
  vtkImageIterateFilter
  vtkImageMagnify
  vtkImageMapToColors
  vtkImageMask
  vtkImageMirrorPad
  vtkImagePadFilter
  vtkImagePermute
  vtkImagePointDataIterator
  vtkImagePointIterator
  vtkImageProbeFilter
  vtkImageResample
  vtkImageResize
  vtkImageReslice
  vtkImageResliceToColors
  vtkImageShiftScale
  vtkImageShrink3D
  vtkImageSincInterpolator
  vtkImageStencilAlgorithm
  vtkImageStencilData
  vtkImageStencilIterator
  vtkImageStencilSource # Needed by vtkImageStencilData
  vtkImageSSIM
  vtkImageThreshold
  vtkImageTranslateExtent
  vtkImageWrapPad
  vtkRTAnalyticSource)

vtk_module_add_module(VTK::ImagingCore
  CLASSES ${classes})
vtk_add_test_mangling(VTK::ImagingCore)

# FIXME: vtkGenericImageInterpolator.cxx:848:8: error: redefinition of 'struct {anonymous}::vtkInterpolateNOP<F>'
set_property(SOURCE vtkGenericImageInterpolator.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
