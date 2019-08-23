# Script that shows the result from redistribution filter
from paraview.simple import *

def print_help():
    print(
    '''
    Usage:
    If in Paraview's build director, in terminal, run
    ./bin/pvpython stem_histogram.py [data_dir -- path to the data directory]
    ''')

def main(argv):
    print_help()
    if (len(argv) == 0):
        data_dir = '/home/zhuokai/Desktop/disk_out_ref.ex2'
    else:
        data_dir = argv[0]

    # load data
    reader = OpenDataFile(data_dir)
    print('Data located at ' + data_dir + ' has been loaded')

    # process pipeline
    # Merge Block filter
    data = GetActiveSource()
    merged_block = MergeBlocks(Input=data)

    # D4 (redistribute filter)
    d41 = D4(Input=merged_block)
    d41.BoundaryMode = 'Split cells'
    d41.NumberOfPartitions = 8
    d41.PreservePartitionsInOutput = True

    # get active view
    render_view = GetActiveViewOrCreate('RenderView')
    # set view size
    render_view.ViewSize = [960, 640]

    # show data in view
    display = Show(d41, render_view)

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
    main(sys.argv[1:])
