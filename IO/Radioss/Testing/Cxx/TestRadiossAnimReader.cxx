// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include <vtkCellData.h>
#include <vtkDataAssembly.h>
#include <vtkInformation.h>
#include <vtkMathUtilities.h>
#include <vtkPartitionedDataSet.h>
#include <vtkPartitionedDataSetCollection.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkRadiossAnimReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkTable.h>
#include <vtkTesting.h>

#include <cstdlib>

int TestRadiossAnimReader(int argc, char* argv[])
{
  vtkNew<vtkTesting> testHelper;
  testHelper->AddArguments(argc, argv);
  if (!testHelper->IsFlagSpecified("-D"))
  {
    std::cerr << "Error: -D /path/to/data was not specified.";
    return EXIT_FAILURE;
  }

  std::string dataRoot = testHelper->GetDataRoot();
  /*  if (!TestExampleFile(dataRoot))
    {
      return EXIT_FAILURE;
    }*/

  std::string tempDirectory = testHelper->GetTempDirectory();
  /*  if (!TestEmptyFile(tempDirectory))
    {
      return EXIT_FAILURE;
    }*/

  return EXIT_SUCCESS;
}
