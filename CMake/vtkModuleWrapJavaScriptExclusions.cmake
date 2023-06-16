# Modules that should not be wrapped to javascript.
# This list of modules are all built in a wasm build.
# Right now, all of them are wrappable with some exceptions.
# The troublesome header files are skipped towards the end of this file.
list(APPEND vtk_module_wrap_javascript_skip_modules
  # "VTK::ChartsCore"
  # "VTK::CommonColor"
  # "VTK::CommonComputationalGeometry"
  # "VTK::CommonCore"
  # "VTK::CommonDataModel"
  # "VTK::CommonExecutionModel"
  # "VTK::CommonImplicitArrays"
  # "VTK::CommonMath"
  # "VTK::CommonMisc"
  # "VTK::CommonSystem"
  # "VTK::CommonTransforms"
  # "VTK::DomainsChemistry"
  # "VTK::DomainsChemistryOpenGL2"
  # "VTK::FiltersAMR"
  # "VTK::FiltersCellGrid"
  # "VTK::FiltersCore"
  # "VTK::FiltersDSP"
  # "VTK::FiltersExtraction"
  # "VTK::FiltersFlowPaths"
  # "VTK::FiltersGeneral"
  # "VTK::FiltersGeneric"
  # "VTK::FiltersGeometry"
  # "VTK::FiltersGeometryPreview"
  # "VTK::FiltersHybrid"
  # "VTK::FiltersHyperTree"
  # "VTK::FiltersImaging"
  # "VTK::FiltersModeling"
  # "VTK::FiltersParallel"
  # "VTK::FiltersParallelImaging"
  # "VTK::FiltersPoints"
  # "VTK::FiltersProgrammable"
  # "VTK::FiltersReduction"
  # "VTK::FiltersSMP"
  # "VTK::FiltersSelection"
  # "VTK::FiltersSources"
  # "VTK::FiltersStatistics"
  # "VTK::FiltersTexture"
  # "VTK::FiltersTopology"
  # "VTK::FiltersVerdict"
  # "VTK::IOAsynchronous"
  # "VTK::IOCellGrid"
  # "VTK::IOChemistry"
  # "VTK::IOCityGML"
  # "VTK::IOCore"
  # "VTK::IOEnSight"
  # "VTK::IOExport"
  # "VTK::IOExportGL2PS"
  # "VTK::IOExportPDF"
  # "VTK::IOGeometry"
  # "VTK::IOImage"
  # "VTK::IOImport"
  # "VTK::IOInfovis"
  # "VTK::IOLSDyna"
  # "VTK::IOLegacy"
  # "VTK::IOMotionFX"
  # "VTK::IOMovie"
  # "VTK::IOOggTheora"
  # "VTK::IOPLY"
  # "VTK::IOParallel"
  # "VTK::IOParallelXML"
  # "VTK::IOSegY"
  # "VTK::IOTecplotTable"
  # "VTK::IOVideo"
  # "VTK::IOXML"
  # "VTK::IOXMLParser"
  # "VTK::ImagingColor"
  # "VTK::ImagingCore"
  # "VTK::ImagingFourier"
  # "VTK::ImagingGeneral"
  # "VTK::ImagingHybrid"
  # "VTK::ImagingMath"
  # "VTK::ImagingMorphological"
  # "VTK::ImagingSources"
  # "VTK::ImagingStatistics"
  # "VTK::ImagingStencil"
  # "VTK::InfovisCore"
  # "VTK::InfovisLayout"
  # "VTK::InteractionImage"
  # "VTK::InteractionStyle"
  # "VTK::InteractionWidgets"
  "VTK::ParallelCore"
  # "VTK::RenderingAnnotation"
  # "VTK::RenderingContext2D"
  # "VTK::RenderingContextOpenGL2"
  # "VTK::RenderingCore"
  # "VTK::RenderingFreeType"
  # "VTK::RenderingGL2PSOpenGL2"
  # "VTK::RenderingHyperTreeGrid"
  # "VTK::RenderingImage"
  # "VTK::RenderingLOD"
  # "VTK::RenderingLabel"
  # "VTK::RenderingOpenGL2"
  # "VTK::RenderingSceneGraph"
  # "VTK::RenderingUI"
  # "VTK::RenderingVolume"
  # "VTK::RenderingVolumeOpenGL2"
  # "VTK::RenderingVtkJS"
  # "VTK::RenderingWebGPU"
  # "VTK::TestingRendering"
  # "VTK::ViewsContext2D"
  # "VTK::ViewsCore"
  # "VTK::ViewsInfovis"
)

# Header files that should not be wrapped to javascript.
list(APPEND vtk_module_wrap_javascript_skip_headers
  # Module: VTK::CommonDataModel
  # Reason: destructor is private
  vtkPolyhedronUtilities
  # Module: VTK::FiltersSMP
  # Reason: destructor is protected and there is no delete function
  vtkSMPMergePolyDataHelper
  # Module: VTK::ImagingCore
  # Reason: destructor is protected and there is no delete function
  vtkImageBSplineInternals
  # Module: VTK::ImagingMathematics
  # Reason: Class provides overloads for SetInputConnection which take precedence for all vtkAlgorithm(s)
  #         Error when calling any filter.SetInputConnection => expected null or instance of vtkImageMathematics, got an instance of vtkObjectBase
  vtkImageMathematics
  # Module: VTK::ImagingMorphological
  # Reason: destructor is protected and there is no delete function
  vtkImageConnector
  # Module: VTK::InteractionWidgets
  # Reason: compiler thinks vtkWidgetEvent is of type vtkWidgetEvent::WidgetEventIds? 
  #         confuses vtkWidgetEvent::Delete() with the enum vtkWidgetEvent::WidgetEventIds::Delete?
  vtkWidgetEvent
)
