#!/usr/bin/env python
import os
import vtk
from vtk.util.misc import vtkGetDataRoot
from vtkhdf_converter import convert

VTK_DATA_ROOT = vtkGetDataRoot()
vtkhdf_converter.convert(VTK_DATA_ROOT + "/Data/mandelbrot.vti")
