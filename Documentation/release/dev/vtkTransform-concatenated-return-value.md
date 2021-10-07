# vtkTransform::GetConcatenatedTransform

The return type of `vtkTransform::GetConcatenatedTransform` has been changed to
be `vtkAbstractTransform*` instead of `vtkLinearTransform*`. While this is an
API break, the act of actually using `vtkLinearTransform*` methods on the
returned pointer were undefined behavior because the actual type returned is a
private type to `vtkTransform`'s implementation. While it is a
`vtkAbstractTransform` it was not actually a `vtkLinearTransform` instance.
Additionally, the documentation mentions that it is a `vtkAbstractTransform`,
so it is suspected that this was a think-o when it was written.
