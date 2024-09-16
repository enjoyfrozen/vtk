set(classes
  vtkBilinearQuadIntersection
  vtkCardinalSpline
  vtkKochanekSpline
  vtkParametricBohemianDome
  vtkParametricBour
  vtkParametricBoy
  vtkParametricCatalanMinimal
  vtkParametricConicSpiral
  vtkParametricCrossCap
  vtkParametricDini
  vtkParametricEllipsoid
  vtkParametricEnneper
  vtkParametricFigure8Klein
  vtkParametricFunction
  vtkParametricHenneberg
  vtkParametricKlein
  vtkParametricKuen
  vtkParametricMobius
  vtkParametricPluckerConoid
  vtkParametricPseudosphere
  vtkParametricRandomHills
  vtkParametricRoman
  vtkParametricSpline
  vtkParametricSuperEllipsoid
  vtkParametricSuperToroid
  vtkParametricTorus)

vtk_module_add_module(VTK::CommonComputationalGeometry
  CLASSES ${classes})
vtk_add_test_mangling(VTK::CommonComputationalGeometry)

# FIXME: vtkParametricSuperToroid.cxx:16:8: error: redefinition of 'double {anonymous}::SgnPower(double, double)'
set_property(SOURCE vtkParametricSuperToroid.cxx
  PROPERTY
    SKIP_UNITY_BUILD_INCLUSION ON)
