# Wrap methods with smart pointers in Python

Methods that contain `vtkSmartPointer<T>` as a parameter or a return value
will no longer be excluded from the Python wrappers.  In addition, the
wrappers will now allow `std::vector<vtkSmartPointer<T>>`, which will be
converted to (or from) a tuple of VTK objects.
