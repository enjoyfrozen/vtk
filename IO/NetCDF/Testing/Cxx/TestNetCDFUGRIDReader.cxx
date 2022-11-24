/*=========================================================================

Program:   Visualization Toolkit
Module:    TestNetCDFUGRIDReader.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkCellData.h"
#include "vtkNetCDFUGRIDReader.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkTesting.h"
#include "vtkUnstructuredGrid.h"

#include <algorithm>

/**
 * Original data:
 *
 * Mesh2 = 0 ;
 * Mesh2_node_x = 0.0, 1.0, 1.0, 0.0,
 *                1.0, 2.0, 2.0 ;
 * Mesh2_node_y = 1.0, 1.0, 0.0, 0.0,
 *                1.0, 1.0, 0.0 ;
 * Mesh2_face_nodes = 1, 2, 3, 4,  //start_index == 1
 *                    5, 6, 7, _ ;
 * h = 0.0, 0.5, 0.2, 0.3, 0.5, 0.0, 0.4,
 *     0.2, 0.3, 0.3, 0.3, 0.2, 0.2, 0.3 ;
 * area = 1.0, 0.5,
 *        0.5, 1.5 ;
 * time = 0, 31 ;
 */

constexpr std::array<std::array<double, 3>, 7> expectedPoints{
  std::array<double, 3>{ 0.0, 1.0, 0.0 },
  std::array<double, 3>{ 1.0, 1.0, 0.0 },
  std::array<double, 3>{ 1.0, 0.0, 0.0 },
  std::array<double, 3>{ 0.0, 0.0, 0.0 },
  std::array<double, 3>{ 1.0, 1.0, 0.0 },
  std::array<double, 3>{ 2.0, 1.0, 0.0 },
  std::array<double, 3>{ 2.0, 0.0, 0.0 },
};

constexpr std::array<vtkIdType, 4> expectedQuadIds{ 0, 1, 2, 3 };
constexpr std::array<vtkIdType, 3> expectedTriangleIds{ 4, 5, 6 };

constexpr std::array<double, 7> expectedPointData{ 0.2, 0.3, 0.3, 0.3, 0.2, 0.2, 0.3 };
constexpr std::array<double, 2> expectedCellData{ 0.5, 1.5 };

#define check(expr, message)                                                                       \
  if (!(expr))                                                                                     \
  {                                                                                                \
    vtkErrorWithObjectMacro(nullptr, << #expr << " | " << message);                                \
    return 1;                                                                                      \
  }                                                                                                \
  (void)0

int TestNetCDFUGRIDReader(int argc, char* argv[])
{
  vtkNew<vtkTesting> testHelper;
  testHelper->AddArguments(argc, argv);
  if (!testHelper->IsFlagSpecified("-D"))
  {
    std::cerr << "Error: -D /path/to/data was not specified.";
    return 1;
  }

  std::string root = testHelper->GetDataRoot();

  vtkNew<vtkNetCDFUGRIDReader> reader;
  reader->SetFileName((root + "/Data/NetCDF/ugrid.nc").c_str());
  reader->UpdateTimeStep(31.0); // use different time
  reader->Update();

  vtkUnstructuredGrid* output = reader->GetOutput();

  // Check cells
  auto cells = output->GetCells();
  check(cells->GetNumberOfCells() == 2, "Wrong number of cells");

  vtkIdType cellSize;
  const vtkIdType* cellIds;

  cells->GetCellAtId(0, cellSize, cellIds);
  check(cellSize == 4, "First cell must be a quad");
  check(std::equal(cellIds, cellIds + 4, expectedQuadIds.begin(), expectedQuadIds.end()),
    "Wrond point ids for first cell");

  cells->GetCellAtId(1, cellSize, cellIds);
  check(cellSize == 3, "Second cell must be a triangle");
  check(std::equal(cellIds, cellIds + 3, expectedTriangleIds.begin(), expectedTriangleIds.end()),
    "Wrond point ids for second cell");

  // Check points
  auto points = output->GetPoints();
  check(points->GetNumberOfPoints() == 7, "Wrong number of points");
  check(points->GetDataType() == VTK_DOUBLE, "Wrong data type for points");

  for (std::size_t i{}; i < expectedPoints.size(); ++i)
  {
    std::array<double, 3> point;
    points->GetPoint(i, point.data());

    check(point == expectedPoints[i], "Wrong point data at point #" << i);
  }

  // Check point data
  auto pointData = output->GetPointData();
  check(pointData->GetNumberOfArrays() == 1, "Wrong number of array");
  check(pointData->HasArray("h"), "Wrong point data array name, must match variable name");
  auto h = pointData->GetArray("h");
  check(h->GetDataType() == VTK_DOUBLE, "Wrong point data array data type");
  check(h->GetNumberOfComponents() == 1, "Wrong point data array number of component");
  check(h->GetNumberOfTuples() == 7, "Wrong point data array number of tuples");
  auto hData = static_cast<double*>(h->GetVoidPointer(0));
  check(std::equal(hData, hData + 7, expectedPointData.begin(), expectedPointData.end()),
    "Wrong point data");

  // Check cell data
  auto cellData = output->GetCellData();
  check(cellData->GetNumberOfArrays() == 1, "Wrong number of array");
  check(cellData->HasArray("area"), "Wrong point data array name, must match variable name");
  auto area = cellData->GetArray("area");
  check(area->GetDataType() == VTK_FLOAT, "Wrong point data array data type");
  check(area->GetNumberOfComponents() == 1, "Wrong point data array number of component");
  check(area->GetNumberOfTuples() == 2, "Wrong point data array number of tuples");
  auto areaData = static_cast<float*>(area->GetVoidPointer(0));
  check(std::equal(areaData, areaData + 2, expectedCellData.begin(), expectedCellData.end()),
    "Wrong point data");

  return 0;
}
