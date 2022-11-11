#!/usr/bin/env python
import sys
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Test the use of seed arrays with the connectivity filters
# (vtkConnectivityFilter and vtkPolyDataConnectivityFilter).

# Construct separate "strips" of cells that represent different
# regions. Seed certain regions with a seed array.
s1 = vtk.vtkPlaneSource()
s1.SetResolution(10,1)
s1.SetOrigin(0,0,0)
s1.SetPoint1(10,0,0)
s1.SetPoint2(0,1,0)

s2 = vtk.vtkPlaneSource()
s2.SetResolution(10,1)
s2.SetOrigin(0,1,0)
s2.SetPoint1(10,1,0)
s2.SetPoint2(0,2,0)

s3 = vtk.vtkPlaneSource()
s3.SetResolution(10,1)
s3.SetOrigin(0,2,0)
s3.SetPoint1(10,2,0)
s3.SetPoint2(0,3,0)

s4 = vtk.vtkPlaneSource()
s4.SetResolution(10,1)
s4.SetOrigin(0,3,0)
s4.SetPoint1(10,3,0)
s4.SetPoint2(0,4,0)

appendF = vtk.vtkAppendPolyData()
appendF.AddInputConnection(s1.GetOutputPort())
appendF.AddInputConnection(s2.GetOutputPort())
appendF.AddInputConnection(s3.GetOutputPort())
appendF.AddInputConnection(s4.GetOutputPort())

# Define seeds for polydata connectivity
seeds = vtk.vtkIdList()
seeds.InsertNextId(0)
seeds.InsertNextId(67)

pConn = vtk.vtkPolyDataConnectivityFilter()
pConn.SetInputConnection(appendF.GetOutputPort())
pConn.SetExtractionModeToPointSeededRegions()
pConn.SetSeeds(seeds)

polyMapper = vtk.vtkPolyDataMapper()
polyMapper.SetInputConnection(pConn.GetOutputPort())

polyActor = vtk.vtkActor()
polyActor.SetMapper(polyMapper)
polyActor.GetProperty().SetColor(0,0,1)

# Define seeds for polydata connectivity
seeds2 = vtk.vtkIdList()
seeds2.InsertNextId(22)
seeds2.InsertNextId(44)

conn = vtk.vtkConnectivityFilter()
conn.SetInputConnection(appendF.GetOutputPort())
conn.SetExtractionModeToPointSeededRegions()
conn.SetSeeds(seeds2)

mapper = vtk.vtkPolyDataMapper()
mapper.SetInputConnection(conn.GetOutputPort())

actor = vtk.vtkActor()
actor.SetMapper(mapper)
actor.GetProperty().SetColor(1,0,0)

# Define graphics objects
ren1 = vtk.vtkRenderer()
ren1.SetBackground(0,0,0)
ren1.AddActor(polyActor)
ren1.AddActor(actor)

renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)

iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

iren.Initialize()
iren.Start()
