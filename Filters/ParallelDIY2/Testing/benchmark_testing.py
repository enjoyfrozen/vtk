# Script that shows the result from redistribution filter
from paraview.simple import *
import numpy as np
import argparse

def main(args):

    method = args.method
    if (method != 'd3' and method != 'd4'):
        print('Invalid method, choose d3 or d4, program stops.')
        return

    data_type = args.data_type
    # check if property type
    if (data_type != 'unstructured' and data_type != 'polygon'):
        print('Invalid requested data type, program stops.')
        return

    # create a Fast Uniform Grid
    uniform_grid = FastUniformGrid()
    # Properties modified on uniform_grid
    uniform_grid.WholeExtent = [0, args.x, 0, args.y, 0, args.z]

    # create 'Tetrahedralize' so that data is unstructured
    tetrahedralize = Tetrahedralize(Input=uniform_grid)

    # apply d4 with unstructured data if told so
    if (data_type == 'unstructured'):
        if (method == 'd3'):
            d = D3(Input=tetrahedralize)
            d.BoundaryMode = 'Divide cells'
        elif (method == 'd4'):
            d = D4(Input=tetrahedralize)
            d.BoundaryMode = 'Split cells'
            d.NumberOfPartitions = args.num_partitions
            d.PreservePartitionsInOutput = True
    elif (data_type == 'polygon'):
        # Use isocontours as 2D polygon data
        contour = Contour(Input=tetrahedralize)
        contour.ContourBy = ['POINTS', 'DistanceSquared']
        contour.Isosurfaces = [5.0]
        contour.PointMergeMethod = 'Uniform Binning'
        if (method == 'd3'):
            d = D3(Input=tetrahedralize)
            d.BoundaryMode = 'Divide cells'
        elif (method == 'd4'):
            d = D4(Input=tetrahedralize)
            d.BoundaryMode = 'Split cells'
            d.NumberOfPartitions = args.num_partitions
            d.PreservePartitionsInOutput = True

    # show the data
    show = int(args.show)
    if (show):
        # get active view
        render_view = GetActiveViewOrCreate('RenderView')
        # set view size
        render_view.ViewSize = [960, 640]
        # show data in view
        display = Show(d, render_view)
        # properties to be rendered
        display.SetRepresentationType('Surface With Edges')
        # need to update before ColorBy
        render_view.Update()
        ColorBy(display, ('FIELD', 'vtkBlockColors'))
        # add a legend bar
        display.SetScalarBarVisibility(render_view, True)
        # render the result
        Render(render_view)
        # keep the window
        Interact()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Bench mark testing script of new redistribution filter.')
    parser.add_argument('method', help='choose redistribution filter d3 or d4')
    parser.add_argument('data_type', help='choose data type, unstructured or polygon')
    parser.add_argument('num_partitions', type=int, help='number of partitions')
    parser.add_argument('show', help='1 for showing result, 0 for not showing result')
    parser.add_argument('x', type=int, help='range of uniform grid in x direction')
    parser.add_argument('y', type=int, help='range of uniform grid in y direction')
    parser.add_argument('z', type=int, help='range of uniform grid in z direction')
    args = parser.parse_args()

    main(args)
