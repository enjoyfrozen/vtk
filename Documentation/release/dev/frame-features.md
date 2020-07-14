## Progress on frame-field and texture-atlas generation

A few small modifications have been made to existing filters
to have them either pass global IDs, create pedigree IDs, or both:

+ vtkExtractEdges (copy global point-IDs if present).
+ vtkFeatureEdges (Add pedigree IDs on request, off by default).
+ vtkFeatureEdges (copy global point IDs if present).
+ vtkGeometryFilter (Add pedigree IDs on request, off by default).

Additionally, VTK now has several new filters:

+ A preliminary vtkMeshReader for reading ASCII NetGen `.mesh` files.
+ `vtkStarIterator` that can be used to traverse the topological "star"
  of a point (i.e., visiting all cells attached to the given point).
+ `vtkDistanceToFeature` that computes the distance from all
  unmarked points of a mesh to the nearest marked point of the mesh
  by traveling along strait lines between points connected by mesh
  cells. Note that this is not a good measure of distance for concave
  cells such as higher-order cells or general polygons/polyhedra.
+ `vtkGrowCharts` that assigns a unique chart ID to each point with
  no existing chart ID such that charts are as large as possible
  while remaining contiguous (i.e., connected by cells to all other
  points with the same chart ID).
+ `vtkPointFeatures` filter creates `VTK_VERTEX` cells at points
  on feature edges that are deemed to be corners by the difference
  in cell normals.
  This filter is useful for providing constraints for frame fields.
