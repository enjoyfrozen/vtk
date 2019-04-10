set (vtkFiltersParallelDIY2CxxTests-MPI_NUMPROCS 1)
vtk_add_test_mpi(vtkFiltersParallelDIY2CxxTests-MPI no_mpi_tests
  TESTING_DATA
  TestRedistributeDataSetFilter.cxx
)

set (vtkFiltersParallelDIY2CxxTests-MPI_NUMPROCS 2)
vtk_add_test_mpi(vtkFiltersParallelDIY2CxxTests-MPI tests
  TESTING_DATA
  TestPResampleToImageCompositeDataSet.cxx
  TestPResampleToImage.cxx
  TestPResampleWithDataSet2.cxx
  TestPResampleWithDataSet.cxx
)

# We want 4 processes to test the vtkAggregateDataSetFilter
# properly.
set (vtkFiltersParallelDIY2CxxTests-MPI_NUMPROCS 4)
vtk_add_test_mpi(vtkFiltersParallelDIY2CxxTests-MPI no_data_tests_4_procs
  DIYAggregateDataSet.cxx
  )

set(all_tests
  ${tests}
  ${no_data_tests_4_procs}
  ${no_mpi_tests}
  )

vtk_test_cxx_executable(vtkFiltersParallelDIY2CxxTests-MPI all_tests)
