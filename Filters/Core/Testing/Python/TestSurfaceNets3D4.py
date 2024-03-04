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

# Extract single region
snets1 = vtk.vtkSurfaceNets3D()
snets1.SetInputData(blobImage)
snets1.GenerateLabels(numBlobs, 1, numBlobs)
snets1.SetConstraintScale(1.5)
snets1.GetSmoother().SetNumberOfIterations(0)
snets1.GetSmoother().SetRelaxationFactor(0.5)
snets1.SetOutputMeshTypeToTriangles()
snets1.SetOutputStyleToExtractSelected()
snets1.AddSelectedLabel(12)
snets1.Update()

mapper1 = vtk.vtkCompositePolyDataMapper2()
mapper1.SetInputConnection(snets1.GetOutputPort(1))

actor1 = vtk.vtkActor()
actor1.SetMapper(mapper1)

# Extract multiple regions
snets2 = vtk.vtkSurfaceNets3D()
snets2.SetInputData(blobImage)
snets2.GenerateLabels(numBlobs, 1, numBlobs)
snets2.SetConstraintScale(1.5)
snets2.GetSmoother().SetNumberOfIterations(0)
snets2.GetSmoother().SetRelaxationFactor(0.5)
snets2.SetOutputMeshTypeToTriangles()
snets2.SetOutputStyleToExtractAll()
snets2.Update()

mapper2 = vtk.vtkCompositePolyDataMapper2()
mapper2.SetInputConnection(snets2.GetOutputPort(1))

actor2 = vtk.vtkActor()
actor2.SetMapper(mapper2)

# Regions are extracted into these
region = vtk.vtkPolyData()
partition = vtk.vtkPartitionedDataSet()

mapper3 = vtk.vtkPolyDataMapper()
mapper3.SetInputData(region)

actor3 = vtk.vtkActor()
actor3.SetMapper(mapper3)

mapper4 = vtk.vtkCompositePolyDataMapper2()
mapper4.SetInputDataObject(partition)

actor4 = vtk.vtkActor()
actor4.SetMapper(mapper4)

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
ren1.SetViewport(0,0,0.5,0.5)
ren2 = vtk.vtkRenderer()
ren2.SetViewport(0.5,0,1,0.5)
ren3 = vtk.vtkRenderer()
ren3.SetViewport(0,0.5,0.5,1)
ren4 = vtk.vtkRenderer()
ren4.SetViewport(0.5,0.5,1,1)

renWin = vtk.vtkRenderWindow()
renWin.SetSize(400,400)
renWin.AddRenderer(ren1)
renWin.AddRenderer(ren2)
renWin.AddRenderer(ren3)
renWin.AddRenderer(ren4)

iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

ren1.AddActor(actor1)
ren1.AddActor(outlineActor)
ren1.ResetCamera()
ren1.GetActiveCamera().Zoom(1.25)

ren2.AddActor(actor2)
ren2.AddActor(outlineActor)
ren2.SetActiveCamera(ren1.GetActiveCamera())

ren3.AddActor(actor3)
ren3.AddActor(outlineActor)
ren3.SetActiveCamera(ren1.GetActiveCamera())

ren4.AddActor(actor4)
ren4.AddActor(outlineActor)
ren4.SetActiveCamera(ren1.GetActiveCamera())

# Extract all regions
snets1.ExtractRegion(12,region)
print("Extract Region: ", region.GetNumberOfPoints(), " points, ", region.GetNumberOfCells(), " cells.")

labels = [12,2,3,4,5,6,7,8,9,10,1,11,13,36]
snets2.ExtractRegions(len(labels),labels,partition,1,1)

renWin.Render()
iren.Start()
