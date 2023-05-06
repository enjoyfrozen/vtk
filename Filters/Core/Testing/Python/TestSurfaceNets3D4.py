#!/usr/bin/env python
import vtk
from math import cos, sin, pi
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

math = vtk.vtkMath()

# Test the ExtractRegion and related methods

# Set the size of the test
dim = 128
numBlobs = 36

# Generate some random colors
def MakeColors (lut, n):
    lut.SetNumberOfColors(n+1)
    lut.SetTableRange(0, n)
    lut.SetScaleToLinear()
    lut.Build()
    lut.SetTableValue(0, 0, 0, 0, 1)
    math.RandomSeed(5071)
    i = 1
    while i < n:
        lut.SetTableValue(i, math.Random(.5, 1),
          math.Random(.5, 1), math.Random(.5, 1), 1)
        i += 1

lut = vtk.vtkLookupTable()
MakeColors(lut, numBlobs)
radius = 10

# Create a bunch of spherical blobs that will be segmented
# out at a later time.
blobImage = vtk.vtkImageData()

i = 0
while i < numBlobs:
    sphere = vtk.vtkSphere()
    sphere.SetRadius(radius)
    max = 50 - radius
    sphere.SetCenter(int(math.Random(-max, max)),
      int(math.Random(-max, max)), int(math.Random(-max, max)))

    sampler = vtk.vtkSampleFunction()
    sampler.SetImplicitFunction(sphere)
    sampler.SetOutputScalarTypeToFloat()
    sampler.SetSampleDimensions(dim, dim, dim)
    sampler.SetModelBounds(-50, 50, -50, 50, -50, 50)

    thres = vtk.vtkImageThreshold()
    thres.SetInputConnection(sampler.GetOutputPort())
    thres.ThresholdByLower(radius * radius)
    thres.ReplaceInOn()
    thres.ReplaceOutOn()
    thres.SetInValue(i + 1)
    thres.SetOutValue(0)
    thres.Update()
    if (i == 0):
        blobImage.DeepCopy(thres.GetOutput())

    maxValue = vtk.vtkImageMathematics()
    maxValue.SetInputData(0, blobImage)
    maxValue.SetInputData(1, thres.GetOutput())
    maxValue.SetOperationToMax()
    maxValue.Modified()
    maxValue.Update()

    blobImage.DeepCopy(maxValue.GetOutput())

    i += 1

angle = pi/6
orientation = [
  -cos(angle), 0, sin(angle),
  0, 1, 0,
  sin(angle), 0, cos(angle),
]
blobImage.SetDirectionMatrix(orientation)

# Extract labeled blobs with no smoothing
snets = vtk.vtkSurfaceNets3D()
snets.SetInputData(blobImage)
snets.GenerateLabels(numBlobs, 1, numBlobs)
snets.SetConstraintScale(1.5)
snets.GetSmoother().SetNumberOfIterations(0)
snets.GetSmoother().SetRelaxationFactor(0.5)
snets.SetOutputMeshTypeToQuads()
snets.SetOutputMeshTypeToTriangles()
snets.Update()

# Blobs are extracted into this
region = vtk.vtkPolyData()
partition = vtk.vtkPartitionedDataSet()

mapper = vtk.vtkPolyDataMapper()
mapper.SetInputData(region)

actor = vtk.vtkActor()
actor.SetMapper(mapper)

mapper2 = vtk.vtkCompositePolyDataMapper2()
mapper2.SetInputDataObject(partition)

actor2 = vtk.vtkActor()
actor2.SetMapper(mapper2)

# Put an outline around it
outline = vtk.vtkImageDataOutlineFilter()
outline.SetInputData(blobImage)

outlineMapper = vtk.vtkPolyDataMapper()
outlineMapper.SetInputConnection(outline.GetOutputPort())

outlineActor = vtk.vtkActor()
outlineActor.SetMapper(outlineMapper)
outlineActor.GetProperty().SetColor(1,1,1)

# Create the RenderWindow, Renderer and both Actors
#
ren1 = vtk.vtkRenderer()
ren1.SetViewport(0,0,0.5,1)
ren2 = vtk.vtkRenderer()
ren2.SetViewport(0.5,0,1,1)
renWin = vtk.vtkRenderWindow()
renWin.SetSize(400,200)
renWin.AddRenderer(ren1)
renWin.AddRenderer(ren2)

iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

ren1.AddActor(actor)
ren1.AddActor(outlineActor)
ren1.ResetCamera()

ren2.AddActor(actor2)
ren2.AddActor(outlineActor)
ren2.SetActiveCamera(ren1.GetActiveCamera())

# Extract all regions
snets.ExtractRegion(12,region)
print("Extract Region: ", region.GetNumberOfPoints(), " points, ", region.GetNumberOfCells(), " cells.")

labels = [12,2,3,4,5,6,7,8,9,10,1,11,13,36]
snets.ExtractRegions(len(labels),labels,partition,1,1)

renWin.Render()
iren.Start()
