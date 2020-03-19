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
plane.SetNormal(1,1,1)

cut = vtk.vtkFlyingEdgesPlaneCutter()
cut.SetInputData(sizeField)
cut.SetPlane(plane)
cut.ComputeNormalsOff()
cut.InterpolateAttributesOn()
cut.Update()

# Now pack the points
smooth = vtk.vtkPointSmoothingFilter()
smooth.SetInputConnection(cut.GetOutputPort())
smooth.SetSmoothingModeToGeometric()
smooth.Update()

sph = vtk.vtkSphereSource()
sph.SetRadius(0.5)
sph.SetCenter(0.0, 0.0, 0.0)

glyph = vtk.vtkTensorGlyph()
glyph.SetInputConnection(smooth.GetOutputPort())
glyph.SetSourceConnection(sph.GetOutputPort())
glyph.SetScaleFactor(0.05)

cutMapper = vtk.vtkPolyDataMapper()
cutMapper.SetInputConnection(glyph.GetOutputPort())

cutActor = vtk.vtkActor()
cutActor.SetMapper(cutMapper)
cutActor.GetProperty().SetColor(1,1,1)
cutActor.GetProperty().SetOpacity(1)

# A outline around the data helps
outline = vtk.vtkOutlineFilter()
outline.SetInputConnection(ptLoad.GetOutputPort())

outlineMapper = vtk.vtkPolyDataMapper()
outlineMapper.SetInputConnection(outline.GetOutputPort())

outlineActor = vtk.vtkActor()
outlineActor.SetMapper(outlineMapper)
outlineActor.GetProperty().SetColor(0,0,0)

# Create the RenderWindow, Renderer and both Actors
#
ren = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren)
iRen = vtk.vtkRenderWindowInteractor()
iRen.SetRenderWindow(renWin)

# Add the actors to the renderer, set the background and size
#
ren.AddActor(cutActor)
ren.AddActor(outlineActor)
renWin.SetSize(300, 300)
ren.SetBackground(1,1,1)

iRen.Initialize()
renWin.Render()

# Actually cut the data
iRen.Start()
