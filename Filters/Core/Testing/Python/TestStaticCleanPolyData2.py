#!/usr/bin/env python
from vtkmodules.vtkCommonCore import (
    vtkIdList,
    vtkMath,
    vtkPoints,
)
from vtkmodules.vtkCommonDataModel import (
    vtkCellArray,
    vtkPolyData,
)
from vtkmodules.vtkCommonSystem import vtkTimerLog
from vtkmodules.vtkFiltersCore import (
    vtkCleanPolyData,
    vtkStaticCleanPolyData,
)

# Create some pathological merging examples and make sure
# that the clean filters properly process them.

# The first polydata just tests coincident points.
pts = vtkPoints()
pts.InsertPoint(0, 0,0,0)
pts.InsertPoint(1, 1,0,0)
pts.InsertPoint(2, 1,1,0)
pts.InsertPoint(3, 0,1,0)
pts.InsertPoint(4, 1,0,0)
pts.InsertPoint(5, 1,1,0)
pts.InsertPoint(6, 0,1,0)
pts.InsertPoint(7, 0,1,0)
pts.InsertPoint(8, 1,1,0)
pts.InsertPoint(9, 0,1,0)

verts = vtkCellArray()
verts.InsertNextCell(1)
verts.InsertCellPoint(9)

lines = vtkCellArray()
lines.InsertNextCell(2)
lines.InsertCellPoint(7)
lines.InsertCellPoint(8)

polys = vtkCellArray()
polys.InsertNextCell(4)
polys.InsertCellPoint(0)
polys.InsertCellPoint(1)
polys.InsertCellPoint(2)
polys.InsertCellPoint(3)

pdata = vtkPolyData()
pdata.SetPoints(pts)
pdata.SetVerts(verts)
pdata.SetLines(lines)
pdata.SetPolys(polys)

ptIds = vtkIdList()

clean = vtkStaticCleanPolyData()
clean.SetInputData(pdata)
clean.Update()

clean.GetOutput().GetVerts().GetCell(0,ptIds)
print("Vert point id: ", ptIds.GetId(0))
assert(ptIds.GetId(0) == 3)

clean.GetOutput().GetLines().GetCell(0,ptIds)
print("Line ids: ", ptIds.GetId(0), ptIds.GetId(1))
assert(ptIds.GetId(0) == 3)
assert(ptIds.GetId(1) == 2)

clean.GetOutput().GetPolys().GetCell(0,ptIds)
print("Poly ids: ", ptIds.GetId(0), ptIds.GetId(1), ptIds.GetId(2), ptIds.GetId(3))
assert(ptIds.GetId(0) == 0)
assert(ptIds.GetId(1) == 1)
assert(ptIds.GetId(2) == 2)
assert(ptIds.GetId(3) == 3)

# The next polydata tests degenerate topology merging
pts2 = vtkPoints()
pts2.InsertPoint(0, 0,0,0)
pts2.InsertPoint(1, 0,0,0)
pts2.InsertPoint(2, 0,1,0)
pts2.InsertPoint(3, 0,0,0)
pts2.InsertPoint(4, 0,0,0)
pts2.InsertPoint(5, 0,0,0)
pts2.InsertPoint(6, 0,0,0)
pts2.InsertPoint(7, 0,0,0)

lines2 = vtkCellArray()
lines2.InsertNextCell(2)
lines2.InsertCellPoint(3)
lines2.InsertCellPoint(4)

polys2 = vtkCellArray()
polys2.InsertNextCell(3)
polys2.InsertCellPoint(0)
polys2.InsertCellPoint(1)
polys2.InsertCellPoint(2)
polys2.InsertNextCell(3)
polys2.InsertCellPoint(5)
polys2.InsertCellPoint(6)
polys2.InsertCellPoint(7)

pdata2 = vtkPolyData()
pdata2.SetPoints(pts2)
pdata2.SetLines(lines2)
pdata2.SetPolys(polys2)

#clean2 = vtkCleanPolyData()
clean2 = vtkStaticCleanPolyData()
clean2.SetInputData(pdata2)
clean2.Update()
cleanOutput = clean2.GetOutput()

print("Number of verts: ", cleanOutput.GetVerts().GetNumberOfCells())
assert(cleanOutput.GetVerts().GetNumberOfCells() == 2)

print("Number of lines: ", cleanOutput.GetLines().GetNumberOfCells())
assert(cleanOutput.GetLines().GetNumberOfCells() == 1)

print("Number of polys: ", cleanOutput.GetPolys().GetNumberOfCells())
assert(cleanOutput.GetPolys().GetNumberOfCells() == 0)

# --- end of script --
