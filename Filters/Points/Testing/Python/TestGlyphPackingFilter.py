#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Interpolate onto a volume

# Parameters for debugging
NPts = 10000
math = vtk.vtkMath()
math.RandomSeed(31415)

# Time execution
timer = vtk.vtkTimerLog()
timer.StartTimer()
//removal.Update()
timer.StopTimer()
time = timer.GetElapsedTime()
print("Time to remove points: {0}".format(time))
print("   Number removed: {0}".format(removal.GetNumberOfPointsRemoved()))
print("   Original number of points: {0}".format(NPts))
