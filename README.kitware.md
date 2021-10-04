# nlohmann json fork for VTK

This branch contains changes required to embed nlohmann json in VTK.

* Ignore whitespace for VTK's commit checks
* Update CMakeLists.vtk.txt to include build/install rules for
  VTK's module system.
* Mangle nlohmann namespace.
