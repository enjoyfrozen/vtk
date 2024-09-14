## Prevent vtkPointSet from returning a null vtkPoints object

The `vtkPointSet` object initializes its `Points` member to a `nullptr`. If
the `GetPoints()` method was subsequently called, it could potentially
return that null pointer. That breaks the contract of the `vtkDataSet`
supperclass, whose documentation specifically states that a `vtkPoints`
object will be created if none is available.

This simple fix for `vtkPointSet::GetPoints()` checks to see if the
`Points` is null. If it is, it creates a new `vtkPoints` object for its
`Points` member. That ensures a valid `vtkPoints` object is always
returned.
