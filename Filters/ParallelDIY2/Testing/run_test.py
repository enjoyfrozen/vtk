import subprocess
import argparse
import time

parser = argparse.ArgumentParser(
        description='Bench mark testing script of new redistribution filter.')
parser.add_argument('method', help='choose redistribution filter d3 or d4')
parser.add_argument('data_type', help='choose data type, unstructured or polygon')
args = parser.parse_args()

# parse input arguments
method = args.method
data_type = args.data_type

# check if property type
if (method != 'd3' and method != 'd4'):
    print('Invalid method, choose d3 or d4, program stops.')
    exit()

if (data_type != 'unstructured' and data_type != 'polygon'):
    print('Invalid requested data type, program stops.')
    exit()

out_file = method + '_stats.txt'

# run tests
print('Running tests with ' + method + ' filter')
start_time = time.time()
for num_partition in range(2, 33):
    print('num_partitions = ' + str(num_partition))
    for cur_range in range(10, 110, 10):
        print('Range = ' + str(cur_range))
        start_time = time.time()
        for i in range(10):
            subprocess.call(['mpirun', '/home/zhuokai/Desktop/paraview-build/bin/pvbatch',
                            '/home/zhuokai/Desktop/paraview/VTK/Filters/ParallelDIY2/Testing/benchmark_testing.py',
                            method, data_type, str(num_partition), '0', str(cur_range), str(cur_range), str(cur_range)])
        print('--- ' + data_type + ' data, num_partition = ' + str(num_partition) + ', Range = ' + str(cur_range) + ', repeated running 10 times took %s seconds ---' % (time.time() - start_time))