#!/usr/bin/env python
# -*- coding: utf-8 -*-

import vtk
import vtk.test.Testing
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Control test resolution
res = 50

# Generate a sizing field. Use a synthetic volume with stress
# tensors.
ptLoad = vtk.vtkPointLoad()
ptLoad.SetModelBounds(-0.5,0.5, -0.5,0.5, -0.5,0.5)
ptLoad.SetSampleDimensions(res,res,res)
ptLoad.Update()

sizeField = ptLoad.GetOutput()
bounds = sizeField.GetBounds()
length = sizeField.GetLength()
center = [(bounds[1]+bounds[0])/2.0, (bounds[3]+bounds[2])/2.0, (bounds[5]+bounds[4])/2.0]

# Cut the data (which has tensors) with a plane
plane = vtk.vtkPlane()
plane.SetOrigin(center)
plane.SetNormal(0,0,1)

cut = vtk.vtkFlyingEdgesPlaneCutter()
cut.SetInputData(sizeField)
cut.SetPlane(plane)
cut.ComputeNormalsOff()
cut.InterpolateAttributesOn()
cut.Update()

# Extract some tensor information
textract = vtk.vtkExtractTensorComponents()
textract.SetInputConnection(cut.GetOutputPort())
textract.ExtractScalarsOn()
textract.ScalarIsDeterminant()
textract.PassTensorsToOutputOn()
textract.Update()

# Now smooth/pack the points in a variety of ways.
# We'll glyph with a transformed sphere
sph = vtk.vtkSphereSource()
sph.SetRadius(0.5)
sph.SetCenter(0.0, 0.0, 0.0)
sph.SetThetaResolution(24)
sph.SetPhiResolution(12)
sph.Update()

# First exercise the default behavior (which is UNIFORM)
smooth0 = vtk.vtkPointSmoothingFilter()
smooth0.SetInputConnection(textract.GetOutputPort())
smooth0.SetSmoothingModeToDefault()
smooth0.SetNumberOfIterations(40)
smooth0.SetNumberOfSubIterations(4)
smooth0.SetRelaxationFactor(0.1)
#smooth0.SetNumberOfIterations(0)
smooth0.SetNeighborhoodSize(12)
smooth0.Update()

glyph0 = vtk.vtkGlyph3D()
glyph0.SetInputConnection(smooth0.GetOutputPort())
glyph0.SetSourceConnection(sph.GetOutputPort())
glyph0.SetScaleFactor(-0.8) #negative to be consistent with normals

gMapper0 = vtk.vtkPolyDataMapper()
gMapper0.SetInputConnection(glyph0.GetOutputPort())
gMapper0.ScalarVisibilityOff()

gActor0 = vtk.vtkActor()
gActor0.SetMapper(gMapper0)
gActor0.GetProperty().SetColor(1,1,1)
gActor0.GetProperty().SetOpacity(1)

# Now the Laplacian behavior
smooth1 = vtk.vtkPointSmoothingFilter()
smooth1.SetInputConnection(textract.GetOutputPort())
smooth1.SetSmoothingModeToLaplacian()
smooth1.SetNumberOfIterations(40)
smooth1.SetNumberOfSubIterations(4)
smooth1.SetRelaxationFactor(0.1)
#smooth1.SetNumberOfIterations(0)
smooth1.SetNeighborhoodSize(12)
smooth1.Update()

glyph1 = vtk.vtkGlyph3D()
glyph1.SetInputConnection(smooth1.GetOutputPort())
glyph1.SetSourceConnection(sph.GetOutputPort())
glyph1.SetScaleFactor(-0.8)

gMapper1 = vtk.vtkPolyDataMapper()
gMapper1.SetInputConnection(glyph1.GetOutputPort())
gMapper1.ScalarVisibilityOff()

gActor1 = vtk.vtkActor()
gActor1.SetMapper(gMapper1)
gActor1.GetProperty().SetColor(1,1,1)
gActor1.GetProperty().SetOpacity(1)

# Now explicitly the Uniform behavior
smooth2 = vtk.vtkPointSmoothingFilter()
smooth2.SetInputConnection(textract.GetOutputPort())
smooth2.SetSmoothingModeToUniform()
smooth2.SetNumberOfIterations(40)
smooth2.SetNumberOfSubIterations(4)
smooth2.SetRelaxationFactor(0.1)
#smooth2.SetNumberOfIterations(0)
smooth2.SetNeighborhoodSize(12)
smooth2.Update()

glyph2 = vtk.vtkGlyph3D()
glyph2.SetInputConnection(smooth2.GetOutputPort())
glyph2.SetSourceConnection(sph.GetOutputPort())
glyph2.SetScaleFactor(-0.8)

gMapper2 = vtk.vtkPolyDataMapper()
gMapper2.SetInputConnection(glyph2.GetOutputPort())
gMapper2.ScalarVisibilityOff()

gActor2 = vtk.vtkActor()
gActor2.SetMapper(gMapper2)
gActor2.GetProperty().SetColor(1,1,1)
gActor2.GetProperty().SetOpacity(1)

# Now explicitly the Scalar behavior
smooth3 = vtk.vtkPointSmoothingFilter()
smooth3.SetInputConnection(textract.GetOutputPort())
smooth3.SetSmoothingModeToScalars()
smooth3.SetNumberOfIterations(40)
smooth3.SetNumberOfSubIterations(4)
smooth3.SetRelaxationFactor(0.1)
#smooth3.SetNumberOfIterations(0)
smooth3.SetNeighborhoodSize(12)
smooth3.Update()

glyph3 = vtk.vtkGlyph3D()
glyph3.SetInputConnection(smooth3.GetOutputPort())
glyph3.SetSourceConnection(sph.GetOutputPort())
glyph3.SetScaleFactor(-0.8)

gMapper3 = vtk.vtkPolyDataMapper()
gMapper3.SetInputConnection(glyph3.GetOutputPort())
gMapper3.ScalarVisibilityOff()

gActor3 = vtk.vtkActor()
gActor3.SetMapper(gMapper3)
gActor3.GetProperty().SetColor(1,1,1)
gActor3.GetProperty().SetOpacity(1)

# Now explicitly the Tensor behavior
smooth4 = vtk.vtkPointSmoothingFilter()
smooth4.SetInputConnection(textract.GetOutputPort())
smooth4.SetSmoothingModeToUniform()
smooth4.SetNumberOfIterations(40)
smooth4.SetNumberOfSubIterations(4)
smooth4.SetRelaxationFactor(0.1)
#smooth4.SetNumberOfIterations(0)
smooth4.SetNeighborhoodSize(12)
smooth4.Update()

glyph4 = vtk.vtkTensorGlyph()
glyph4.SetInputConnection(smooth4.GetOutputPort())
glyph4.SetSourceConnection(sph.GetOutputPort())
glyph4.SetScaleFactor(0.05)

gMapper4 = vtk.vtkPolyDataMapper()
gMapper4.SetInputConnection(glyph4.GetOutputPort())

gActor4 = vtk.vtkActor()
gActor4.SetMapper(gMapper4)
gActor4.GetProperty().SetColor(1,1,1)
gActor4.GetProperty().SetOpacity(1)

# Now explicitly the Frame Field behavior
smooth5 = vtk.vtkPointSmoothingFilter()
smooth5.SetInputConnection(textract.GetOutputPort())
smooth5.SetSmoothingModeToUniform()
smooth5.SetNumberOfIterations(40)
smooth5.SetNumberOfSubIterations(4)
smooth5.SetRelaxationFactor(0.1)
#smooth5.SetNumberOfIterations(0)
smooth5.SetNeighborhoodSize(12)
smooth5.Update()

glyph5 = vtk.vtkTensorGlyph()
glyph5.SetInputConnection(smooth5.GetOutputPort())
glyph5.SetSourceConnection(sph.GetOutputPort())
glyph5.SetScaleFactor(0.05)

gMapper5 = vtk.vtkPolyDataMapper()
gMapper5.SetInputConnection(glyph5.GetOutputPort())

gActor5 = vtk.vtkActor()
gActor5.SetMapper(gMapper5)
gActor5.GetProperty().SetColor(1,1,1)
gActor5.GetProperty().SetOpacity(1)

# A outline around the data helps for context
outline = vtk.vtkOutlineFilter()
outline.SetInputConnection(ptLoad.GetOutputPort())

outlineMapper = vtk.vtkPolyDataMapper()
outlineMapper.SetInputConnection(outline.GetOutputPort())

outlineActor = vtk.vtkActor()
outlineActor.SetMapper(outlineMapper)
outlineActor.GetProperty().SetColor(1,1,1)

# Create the RenderWindow, Renderer and both Actors
#
ren0 = vtk.vtkRenderer()
ren0.SetViewport(0, 0, 0.333, 0.5)
ren1 = vtk.vtkRenderer()
ren1.SetViewport(0.333, 0, 0.667, 0.5)
ren2 = vtk.vtkRenderer()
ren2.SetViewport(0.667, 0, 1, 0.5)
ren3 = vtk.vtkRenderer()
ren3.SetViewport(0, 0.5, 0.333, 1)
ren4 = vtk.vtkRenderer()
ren4.SetViewport(0.333, 0.5, 0.667, 1)
ren5 = vtk.vtkRenderer()
ren5.SetViewport(0.667, 0.5, 1, 1)

renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren0)
renWin.AddRenderer(ren1)
renWin.AddRenderer(ren2)
renWin.AddRenderer(ren3)
renWin.AddRenderer(ren4)
renWin.AddRenderer(ren5)

iRen = vtk.vtkRenderWindowInteractor()
iRen.SetRenderWindow(renWin)

# Add the actors to the renderer, set the background and size
#
camera = vtk.vtkCamera()
camera.SetPosition(0,0,1)

ren0.AddActor(gActor0)
ren0.AddActor(outlineActor)
ren0.SetBackground(0,0,0)
ren0.SetActiveCamera(camera)
ren0.ResetCamera()

ren1.AddActor(gActor1)
ren1.AddActor(outlineActor)
ren1.SetBackground(0,0,0)
ren1.SetActiveCamera(camera)

ren2.AddActor(gActor2)
ren2.AddActor(outlineActor)
ren2.SetBackground(0,0,0)
ren2.SetActiveCamera(camera)

ren3.AddActor(gActor3)
ren3.AddActor(outlineActor)
ren3.SetBackground(0,0,0)
ren3.SetActiveCamera(camera)

ren4.AddActor(gActor4)
ren4.AddActor(outlineActor)
ren4.SetBackground(0,0,0)
ren4.SetActiveCamera(camera)

ren5.AddActor(gActor5)
ren5.AddActor(outlineActor)
ren5.SetBackground(0,0,0)
ren5.SetActiveCamera(camera)

renWin.SetSize(600, 400)

iRen.Initialize()
renWin.Render()

# Actually cut the data
iRen.Start()
