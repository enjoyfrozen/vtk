#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Manually create a sample image.
VTK_SHORT = 4
xDim = 3
yDim = 5
zDim = 7
yDim = 3
zDim = 3
sliceSize = xDim * yDim
numVoxels = xDim * yDim * zDim
image = vtk.vtkImageData()
image.SetDimensions(xDim,yDim,zDim)
image.AllocateScalars(VTK_SHORT,1)

imMapper = vtk.vtkDataSetMapper()
imMapper.SetInputData(image)

imActor = vtk.vtkActor()
imActor.SetMapper(imMapper)

# Fill the scalars with 0 and then set particular values.
# Here we'll create several regions / labels.
def GenIndex(i,j,k):
    return i + j*xDim + k*sliceSize
scalars = image.GetPointData().GetScalars()
scalars.Fill(0)

# For debugging, assign scalar values
# consistent with voxel number
#for idx in range(0,numVoxels):
#    scalars.SetTuple1(idx,idx)

# Region 1
scalars.SetTuple1(GenIndex(1,1,1),1)

# Extract the boundaries of labels 1 with SurfaceNets. In this test,
# it should just produce a hex around the single labeled point. Also
# disable smoothing it makes no sense in this situation.
snets = vtk.vtkSurfaceNets3D()
snets.SetInputData(image)
snets.SetValue(0,1)
snets.GetSmoother().SetNumberOfIterations(0)
snets.GetSmoother().SetRelaxationFactor(0.2)
snets.GetSmoother().SetConstraintDistance(0.25)

timer = vtk.vtkTimerLog()
timer.StartTimer()
snets.Update()
timer.StopTimer()
time = timer.GetElapsedTime()
print("Time to generate Surface Net: {0}".format(time))

print(snets.GetOutput())
w = vtk.vtkPolyDataWriter()
w.SetInputConnection(snets.GetOutputPort())
w.SetFileName("out.vtk")
#w.Write()

# Clipped polygons are generated
mapper = vtk.vtkPolyDataMapper()
mapper.SetInputConnection(snets.GetOutputPort())
mapper.SetScalarModeToUseCellData()
mapper.SelectColorArray("BoundaryLabels")
mapper.SetScalarRange(0,4)

actor = vtk.vtkActor()
actor.SetMapper(mapper)
actor.GetProperty().SetInterpolationToFlat()

# Create the RenderWindow, Renderer and both Actors
#
ren1 = vtk.vtkRenderer()
ren1.SetBackground(0,0,0)
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

ren1.AddActor(actor)
#ren1.AddActor(imActor) #uncomment to see image

renWin.Render()
iren.Start()
