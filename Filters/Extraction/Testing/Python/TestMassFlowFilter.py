#!/usr/bin/env python
import vtk
import vtk.test.Testing
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Create a simulated flow through a sphere of radius=1 with density values=1. The velocity values
# are the same as the sphere normals.
sphere = vtk.vtkSphereSource()
sphere.SetRadius(1.0)
sphere.SetThetaResolution(32)
sphere.SetPhiResolution(16)
sphere.Update()
output = sphere.GetOutput()

# Add a velocity and density array.
numPts = output.GetNumberOfPoints()
dens = vtk.vtkFloatArray()
dens.SetName("Density")
dens.SetNumberOfComponents(1)
dens.SetNumberOfTuples(numPts)
vel = vtk.vtkDoubleArray()
vel.SetName("Velocity")
vel.SetNumberOfComponents(3)
vel.SetNumberOfTuples(numPts)
normals = output.GetPointData().GetNormals()

for i in range(0,numPts):
    dens.SetTuple1(i,1.0)
    vel.SetTuple(i, normals.GetTuple(i))

output.GetPointData().AddArray(dens)
output.GetPointData().AddArray(vel)

# Now compute mass flow
# Should be nonzero (~numPts)
mff = vtk.vtkMassFlowFilter()
mff.SetInputData(output)
mff.SetDirectionModeToSurfaceNormals()
mff.SetDensityArrayName("Density")
mff.SetVelocityArrayName("Velocity")
mff.SetMassFlowArrayName("Mass Flow")
mff.Update()
total = mff.GetTotalMassFlow()
print("Total mass flow: {0}".format(total))
assert( total >= numPts-1 and total <= numPts+1 )

# Should be zero
mff.SetDirectionModeToSpecifiedDirection()
mff.SetDirection(0,0,1)
mff.Update()
total = mff.GetTotalMassFlow()
print("Total (directional) mass flow: {0}".format(total))
assert( total > (-1) and total < 1 )

# Remove normals so they have to be calculated. Again should be ~numPts.
output.GetPointData().RemoveArray("Normals")
mff.SetDirectionModeToSurfaceNormals()
mff.Update()
total = mff.GetTotalMassFlow()
print("Total (normals calculated) mass flow: {0}".format(total))
assert( total >= numPts-1 and total <= numPts+1 )

# Now create an integer density array, twice as dense. Should give a value
# ~2*numPts
intDens = vtk.vtkIntArray()
intDens.SetName("Int Density")
intDens.SetNumberOfComponents(1)
intDens.SetNumberOfTuples(numPts)

for i in range(0,numPts):
    intDens.SetTuple1(i,2)

output.GetPointData().AddArray(intDens)

mff.SetDensityArrayName("Int Density")
mff.SetDirectionModeToSurfaceNormals()
mff.Update()
total = mff.GetTotalMassFlow()
print("Total (generalized) mass flow: {0}".format(total))
assert( total >= 2*numPts-1 and total <= 2*numPts+1 )
