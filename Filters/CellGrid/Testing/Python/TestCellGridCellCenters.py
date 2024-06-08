# SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
# SPDX-License-Identifier: BSD-3-Clause

from vtkmodules import vtkCommonCore as cc
from vtkmodules import vtkCommonDataModel as dm
from vtkmodules import vtkCommonExecutionModel as em
from vtkmodules import vtkImagingCore as ic
from vtkmodules import vtkFiltersCellGrid as fc
from vtkmodules import vtkIOCellGrid as io
from vtkmodules import vtkIOXML as ix
from vtkmodules import vtkIOIOSS as ii
from vtkmodules import vtkCommonColor as cl
from vtkmodules.util.vtkAlgorithm import VTKPythonAlgorithmBase

from vtkmodules.vtkRenderingCore import *
from vtkmodules.vtkRenderingOpenGL2 import *
from vtkmodules.vtkRenderingCellGrid import *
from vtkmodules.vtkInteractionStyle import *
from vtkmodules.vtkInteractionWidgets import vtkCameraOrientationWidget

from vtkmodules.test import Testing
import os
import sys

# Register render responder for DG cells:
vtkRenderingCellGrid.RegisterCellsAndResponders()
fc.vtkFiltersCellGrid.RegisterCellsAndResponders()

class TestCellGridCellCenters(Testing.vtkTest):
    def testCellCenters(self):
        bds = [0, 0, 0, 0, 0, 0]

        rdr = io.vtkCellGridReader()
        rdr.SetFileName(os.path.join(Testing.VTK_DATA_ROOT, 'Data', 'fandisk.dg'))
        ctr = fc.vtkCellGridCellCenters()
        ctr.SetInputConnection(rdr.GetOutputPort())

        srf = fc.vtkCellGridComputeSides()
        srf.SetInputConnection(rdr.GetOutputPort())
        sid = fc.vtkCellGridComputeSides()
        sid.SetOutputDimensionControl(dm.vtkCellGridSidesQuery.EdgesOfInputs)
        # sid.OmitSidesForRenderableInputsOff()
        sid.SetInputConnection(srf.GetOutputPort())
        sid.Update()

        shapeMapper = vtkCellGridMapper()
        shapeMapper.SetInputConnection(sid.GetOutputPort())
        shapeMapper.ScalarVisibilityOff()
        shapeActor = vtkActor()
        shapeActor.SetMapper(shapeMapper)
        sppty = vtkProperty()
        sppty.SetOpacity(1.0)
        sppty.SetLineWidth(1)
        sppty.SetPointSize(8)
        sppty.SetColor(0, 0, 0)
        shapeActor.SetProperty(sppty)

        ctr.Update()
        centerMapper = vtkCellGridMapper()
        centerMapper.SetInputConnection(ctr.GetOutputPort())
        centerMapper.ScalarVisibilityOff()
        centerActor = vtkActor()
        centerActor.SetMapper(centerMapper)
        cppty = vtkProperty()
        cppty.SetOpacity(1.0)
        cppty.SetLineWidth(5)
        cppty.SetPointSize(8)
        cppty.SetColor(1, 0, 0)
        centerActor.SetProperty(cppty)

        ren = vtkRenderer()
        ren.AddActor(shapeActor)
        ren.AddActor(centerActor)

        rw = vtkRenderWindow()
        rw.AddRenderer(ren)
        ren.SetBackground(1.0, 1.0, 1.0)
        #ren.SetBackground(0.5, 0.4, 0.3)
        #ren.SetBackground2(0.7, 0.6, 0.5)
        #ren.GradientBackgroundOn()

        rwi = rw.MakeRenderWindowInteractor()# vtkRenderWindowInteractor()
        rwi.SetRenderWindow(rw)

        cow = vtkCameraOrientationWidget()
        cow.SetParentRenderer(ren)
        # Enable the widget.
        cow.On()

        sid.GetOutputDataObject(0).GetBounds(bds)
        cam = ren.GetActiveCamera()
        # If you want to change the camera location, this is a good starting point:
        # dc = ((bds[0]+bds[1])/2, (bds[2]+bds[3])/2, (bds[4] + bds[5])/2)
        # cam.SetFocalPoint(dc[0], dc[1], dc[2] + 1)
        # cam.SetPosition(dc[0]+1, dc[1]+1, dc[2]-1.)
        cam.SetViewUp(0., 0., -1.)
        cam.SetPosition(2.21346, 2.24257, -3.66879)
        cam.SetFocalPoint(0.146239, 0.394899, 0.0458881)
        cam.SetDistance(4.63531)

        rwi.Initialize()
        rw.Render()
        if '-I' in sys.argv:
            rwi.Start()
            print('camera', cam)
        baseline = 'TestCellGridCellCenters.png'
        Testing.compareImage(rw, Testing.getAbsImagePath(baseline), threshold=25)

if __name__ == "__main__":
    Testing.main([(TestCellGridCellCenters, 'test')])
