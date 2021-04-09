Extend and apply the PreserveTopology feature for much faster cell filtering. This allows multiple selection filters to indicate whether a cell should be filtered or not, and have another filter later in the pipeline actually do the filtering. This reduces the number of filters which need to scan and copy all the cell and points arrays; a significant performance improvement.

1. Allow filtering arrays to be named and combined so filters can be run sequentially.
1. Add filtering to the surface filter to filter cells based on the inclusion flags
1. Add filtering to cutting filters so cut planes through filtered cells will generate filtered surfaces
1. Add an option to merge cut faces into polygons. The cutting filters often generate triangulated cells, which can add extra gridlines which could confuse a user as to the bounds of the original cell. This merges the triangulation back together to remove these synthetic gridlines, while keeping the cut cell shapes.
