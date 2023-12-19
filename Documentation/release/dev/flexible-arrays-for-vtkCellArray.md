## Flexible arrays for vtkCellArray

You can now specify different types of vtkDataArray instances for the connectivity and offsets in a `vtkCellArray`
with `vtkCellArray::SetData(vtkDataArray* offsets, vtkDataArray* conn)` and not worry about VTK deep copying the data arrays.

VTK algorithm developers should now keep in mind that the data array type of connectivity and offsets might not always be the same.
Here's a well-written visitor suitable for the `vtkCellArray::Visit` API.

As an example, a visitor can get the value types of connectivity and offset arrays as shown below.
```c++
struct MyVisitor
{
  template <typename CellStateT>
  void operator()(CellStateT& state, vtkIdType cellId) const
  {
    using ConnectivityValueType = typename CellStateT::ConnectivityValueType;
    using OffsetsValueType = typename CellStateT::OffsetsValueType;
    // ...
  }
};
```
