#!/usr/bin/env python
'''
Test converting vti, vtu and pvtu to hdf format.
'''
from subprocess import Popen, PIPE
import sys
from vtk.util.misc import vtkGetDataRoot, vtkGetTempDir
from vtkhdf_converter import convert


def compare(original_file, converted_file):
    '''
    Compare two HDF files
    '''
    h5diff_executable = sys.argv[1]
    process = Popen([h5diff_executable, original_file, converted_file],
                    stdout=PIPE)
    (output, err) = process.communicate()
    exit_code = process.wait()
    if exit_code:
        print(output)
        print(err)
    return exit_code


VTK_DATA_ROOT = vtkGetDataRoot()
VTK_TEMP_DIR = vtkGetTempDir()
FILES = [("mandelbrot.vti", "mandelbrot-vti.hdf"),
         ("can.vtu", "can-vtu.hdf"),
         ("can.pvtu", "can-pvtu.hdf")]

for (vtk_file, hdf_file) in FILES:
    convert(VTK_DATA_ROOT + "/Data/" + vtk_file,
            VTK_TEMP_DIR + "/" + hdf_file)
    if (compare(VTK_DATA_ROOT + "/Data/" + hdf_file,
                VTK_TEMP_DIR + "/" + hdf_file)):
        print("Error: {} is different than expected file {}".format(
            VTK_TEMP_DIR + "/" + hdf_file,
            VTK_DATA_ROOT + "/Data/" + hdf_file))
        sys.exit(1)
