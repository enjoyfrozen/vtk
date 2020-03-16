#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
=========================================================================

  Program:   Visualization Toolkit
  Module:    TestPointCloudWidget.py

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================
'''

import vtk
import vtk.test.Testing
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Demonstrate how to use the vtkPointCloudWidget.
# This script uses a 3D point cloud widget to select and manipulate
# a point. Make sure that you hit the "W" key to activate the widget.

# control the size of the test
npts = 10000

# create a point source
#
pc = vtk.vtkPointSource()
pc.SetNumberOfPoints(npts)
pc.SetCenter(5,10,20)
pc.SetRadius(7.5)

pcMapper = vtk.vtkPointGaussianMapper()
pcMapper.SetInputConnection(pc.GetOutputPort())
pcMapper.EmissiveOff()
pcMapper.SetScaleFactor(0.0)

pcActor = vtk.vtkActor()
pcActor.SetMapper(pcMapper)
pcActor.VisibilityOn()

# Create the RenderWindow, Renderer and both Actors
#
ren = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren)

iRen = vtk.vtkRenderWindowInteractor()
iRen.SetRenderWindow(renWin);

ren.AddActor(pcActor)

# Add the actors to the renderer, set the background and size
#
ren.SetBackground(0.1, 0.2, 0.4)
renWin.SetSize(300, 300)

def SelectPoint(widget, event_string):
    pcWidget,rep
    print("Point Id {0}: ".format(rep.GetPointId()))

rep = vtk.vtkPointCloudRepresentation()
rep.SetPlaceFactor(1.0);
rep.SetPointCloudActor(pcActor);

pcWidget = vtk.vtkPointCloudWidget()
pcWidget.SetInteractor(iRen)
pcWidget.SetRepresentation(rep);
pcWidget.AddObserver("PickEvent",SelectPoint);
pcWidget.On()

recorder = vtk.vtkInteractorEventRecorder()
recorder.SetInteractor(iRen)
recorder.ReadFromInputStringOn()
#recorder.SetInputString(Recording)

# render and interact with data
renWin.Render()
#recorder.Play()
iRen.Start()
