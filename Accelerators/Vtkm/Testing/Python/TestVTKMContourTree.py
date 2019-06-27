import sys
import vtk

mpCont = vtk.vtkMultiProcessController.GetGlobalController()
if not mpCont:
  numProcs = 1
  rank = 0
else:
  numProcs = mpCont.GetNumberOfProcesses()
  rank = mpCont.GetLocalProcessId()

# setup parallel rendering
prm = vtk.vtkCompositeRenderManager()
renderer = prm.MakeRenderer()
renWin = prm.MakeRenderWindow()
renWin.AddRenderer(renderer)

iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

prm.SetRenderWindow(renWin)
prm.SetController(mpCont)

# pipeline

# RTAnalyticSource=======================================================
et = vtk.vtkExtentTranslator()
et.SetWholeExtent(0, 31, 0, 31, 0, 31)
et.SetNumberOfPieces(numProcs)
et.SetPiece(rank)
et.SetSplitModeToZSlab()
et.PieceToExtent()

source = vtk.vtkRTAnalyticSource()
source.SetWholeExtent(0, 31, 0, 31, 0, 31)
source.SetCenter(16, 16, 16)
source.UpdateExtent(et.GetExtent())

inDataSet = source.GetOutput()
fieldName = "RTData"

# Ex_500.vtk=============================================================
# source = vtk.vtkDataSetReader()
# source.SetFileName("Ex_500.vtk")
# source.Update()

# inDataSet = source.GetOutput()
# print inDataSet.GetExtent()

# et = vtk.vtkExtentTranslator()
# et.SetWholeExtent(inDataSet.GetExtent())
# et.SetNumberOfPieces(numProcs)
# et.SetPiece(rank)
# et.SetSplitModeToZSlab()
# et.PieceToExtent()

# voi = vtk.vtkExtractVOI()
# voi.SetInputConnection(source.GetOutputPort())
# voi.SetVOI(et.GetExtent())
# voi.Update()

# fieldName = "Ex"

# inDataSet = voi.GetOutput()
# inDataSet.GetPointData().RemoveArray("vtkValidPointMask")
# inDataSet.GetPointData().SetActiveScalars(fieldName)
# print inDataSet.GetExtent()
#========================================================================

contourTree = vtk.vtkmContourTree()
contourTree.SetInputData(inDataSet)
contourTree.SetNumberOfIsoValues(15)
contourTree.SetIsoValuesType(0)

mapper = vtk.vtkPolyDataMapper()
mapper.SetInputConnection(contourTree.GetOutputPort())
mapper.SetScalarModeToUsePointFieldData()
mapper.SelectColorArray(fieldName)
mapper.SetScalarRange(0, 255)
mapper.Update()

print contourTree.GetIsoValues()

actor = vtk.vtkActor()
actor.SetMapper(mapper)
renderer.AddActor(actor)

if rank == 0:
  prm.ResetAllCameras()
  renWin.Render()
  prm.StartInteractor()
  mpCont.TriggerBreakRMIs()
else:
  prm.StartServices()

writer = vtk.vtkPDataSetWriter()
writer.SetFileName("isosurfaces.vtk")
writer.SetInputConnection(contourTree.GetOutputPort())
writer.SetNumberOfPieces(numProcs)
writer.SetStartPiece(rank)
writer.SetEndPiece(rank)
writer.Write()
