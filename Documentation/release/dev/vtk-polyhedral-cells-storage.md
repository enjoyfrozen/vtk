## Improvements related to vtk Polyhedral cell storage

###  Storing faces as vtkCellArray

When storing face information for polyhedral cells, `vtkUnstructuredGrid` uses a `vtkIdTypeArray` in a special format that looks awfully similar to how `vtkCellArray` used maintain its internal storage.
The new internal storage simply changes it to using `vtkCellArray` instead.
That will allow us to use a non-interleaved storage for faces. Together with using a `vtkCellArray` for `FaceLocations`  the one-2-many relationship can be easily preserved without resorting to custom layout used by the `Faces` array.

So here's the proposed structure:
* **Connectivity** (`vtkCellArray`): simply stores point ids for all points for each polyhedral element
* **PolyhedronFaces** (`vtkCellArray`): each element defines a face. The indices per element directly refer to point ids.
* **PolyhedronFaceLocations** (`vtkCellArray`): each element identifies a polyhedral cell. The indices per element reference face defined in the **PolyhedronFace** array.


Contrast this with how this information is currently stored:
* **Connectivity** (`vtkCellArray`): simply stores point ids for all points for each polyhedral element (same as in the new proposal)
* **Faces** (`vtkIdTypeArray`): an interlaved array of the form `(numCellFaces, numFace0Pts, id1, id2, id3, numFace1Pts,id1, id2, id3, ...)`
* **FaceLocations** (`vtkIdTypeArray`): offset array into **Faces** array indicating where the faces for a corresponding cell are stored


### Backward compatibility

To ensure a nice transition to the new storage, old API are kept.
An internal cache with the old internal layout is used.
Thus it may impact a bit performance.
