vtk_object_factory_declare(
  BASE      vtkResampleToImage
  OVERRIDE  vtkPResampleToImage)
vtk_object_factory_declare(
  BASE      vtkResampleWithDataSet
  OVERRIDE  vtkPResampleWithDataSet)

set(classes
  vtkPResampleToImage
  vtkPResampleWithDataSet
  vtkRedistributeDataSetFilter
  vtkPlaneClipDataSet)

if (TARGET VTK::ParallelMPI)
  # FIXME: vtkDIYAggregateDataSetFilter should not require MPI
  # current implementation does (ref: paraview/paraview#19098)
  vtk_object_factory_declare(
    BASE      vtkAggregateDataSetFilter
    OVERRIDE  vtkDIYAggregateDataSetFilter)
  list(APPEND classes
    vtkDIYAggregateDataSetFilter)
endif()

vtk_object_factory_configure(
  SOURCE_FILE vtk_object_factory_source
  HEADER_FILE vtk_object_factory_header
  EXPORT_MACRO "VTKFILTERSPARALLELDIY2_EXPORT")

message("vtk_object_factory_configure contains:")
message(${vtk_object_factory_source})

vtk_module_add_module(VTK::FiltersParallelDIY2
  CLASSES ${classes}
  SOURCES ${vtk_object_factory_source} vtkDIYKdTreeUtilities.cxx vtkDIYUtilities.cxx vtkPlaneClipDataSet.cxx
  PRIVATE_HEADERS ${vtk_object_factory_header} vtkDIYKdTreeUtilities.h vtkDIYUtilities.h vtkPlaneClipDataSet.h)

vtk_module_definitions(VTK::FiltersParallelDIY2
  PRIVATE DIY_NO_THREADS)
