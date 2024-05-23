set(headers
  vtkDebugBreakpoint.h)
vtk_module_add_module(VTK::debugging
  HEADERS     ${headers}
  NO_INSTALL
  HEADER_ONLY)
