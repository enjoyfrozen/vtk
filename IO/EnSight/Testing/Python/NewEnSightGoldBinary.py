#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# create a rendering window and renderer
ren1 = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
renWin.SetSize(300,300)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

reader = vtk.vtkNewEnSightGoldReader()
cdp = vtk.vtkCompositeDataPipeline()
reader.SetDefaultExecutivePrototype(cdp)
reader.SetCaseFileName("" + str(VTK_DATA_ROOT) + "/Data/EnSight/new-ensight-gold-test-bin.case")
geom0 = vtk.vtkGeometryFilter()
geom0.SetInputConnection(reader.GetOutputPort())
mapper0 = vtk.vtkHierarchicalPolyDataMapper()
mapper0.SetInputConnection(geom0.GetOutputPort())
actor0 = vtk.vtkActor()
actor0.SetMapper(mapper0)
# assign our actor to the renderer
ren1.AddActor(actor0)
# enable user interface interactor
iren.Initialize()
# prevent the tk window from showing up then start the event loop
reader.SetDefaultExecutivePrototype(None)
