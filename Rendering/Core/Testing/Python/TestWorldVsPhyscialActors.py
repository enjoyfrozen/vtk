#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

cone1 = vtk.vtkConeSource()
cone1.SetResolution(32)

cone2 = vtk.vtkConeSource()
cone2.SetResolution(32)

mapper1 = vtk.vtkMapper()
mapper1.SetInputConnection(cone1.GetOutputPort())

mapper2 = vtk.vtkMapper()
mapper2.SetInputConnection(cone1.GetOutputPort())

worldActor = vtk.vtkActor()
worldActor.SetMapper(mapper1)

physicalActor = vtk.vtkActor()
physicalActor.SetMapper(mapper2)

ren1 = vtk.vtkRenderer()

ren1.AddActor(worldActor)
ren1.AddActor(physicalActor)

# Red cone in world coordinates (the default)
worldActor.GetProperty().SetColor(1.0, 0.0, 0.0)

# Green cone in physical coordinates
physicalActor.GetProperty().SetColor(0.0, 1.0, 0.0)
physicalActor.SetCoordinateSystemToPhysical()
physicalActor.SetCoordinateSystemRenderer(ren1)

renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
renWin.SetSize(512,512)

p2w = renWin.GetPhysicalToWorldMatrix()
tform = vtk.vtkTransform()
tform.Identity()
tform.Translate(1.0, 0, 0)
p2w.Multiply4x4(tform.GetMatrix(), p2w, p2w)
renWin.SetPhysicalToWorldMatrix(p2w)

iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
iren.Initialize()
renWin.Render()
# --- end of script --