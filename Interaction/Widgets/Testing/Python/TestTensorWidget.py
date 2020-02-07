#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
=========================================================================

  Program:   Visualization Toolkit
  Module:    TestTensorWidget.py

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

class TestTensorWidget(vtk.test.Testing.vtkTest):

    def testTensorWidget(self):

        # Demonstrate how to use the vtkTensorWidget This script uses a
        # vtkTensorWidget and vtkTensorRepresentation to select and edit a
        # tensor and associated glyph. Make sure that you hit the "I" key to
        # activate the widget.

        # Create the RenderWindow, Renderer and both Actors
        #
        ren = vtk.vtkRenderer()
        renWin = vtk.vtkRenderWindow()
        renWin.AddRenderer(ren)

        iRen = vtk.vtkRenderWindowInteractor()
        iRen.SetRenderWindow(renWin);

        # Define callback for the widget
        def SelectPolygons(widget, event_string):
            '''
            The callback takes two parameters.
            Parameters:
              widget - the object that generates the event.
              event_string - the event name (which is a string).
            '''
            tsWidget

        # Create a representation for the widget
        bbox = [-1,1,-1,1,-1,1]
        rep = vtk.vtkTensorRepresentation()
        tensProp = rep.GetEllipsoidProperty()
        tensProp.SetColor(0.4,0.4,0.8)
        tensProp.SetRepresentationToWireframe()
        rep.SetPlaceFactor(1)
        rep.PlaceWidget(bbox)

        # The widget proper
        tsWidget = vtk.vtkTensorWidget()
        tsWidget.SetInteractor(iRen)
        tsWidget.SetRepresentation(rep)
        tsWidget.AddObserver("EndInteractionEvent", SelectPolygons)
        tsWidget.On()

        # Add the actors to the renderer, set the background and size
        #
        ren.SetBackground(0.1, 0.2, 0.4)
        renWin.SetSize(300, 300)

        # render and interact with data
        ren.ResetCamera()
        renWin.Render()
        iRen.Start()

        img_file = "TestTensorWidget.png"
        vtk.test.Testing.compareImage(iRen.GetRenderWindow(), vtk.test.Testing.getAbsImagePath(img_file), threshold=25)
        vtk.test.Testing.interact()

if __name__ == "__main__":
     vtk.test.Testing.main([(TestTensorWidget, 'test')])
