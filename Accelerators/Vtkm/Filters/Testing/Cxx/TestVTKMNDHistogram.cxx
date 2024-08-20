// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkmNDHistogram.h"

#include "vtkActor.h"
#include "vtkArrayData.h"
#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkDelaunay3D.h"
#include "vtkDoubleArray.h"
#include "vtkImageData.h"
#include "vtkImageToPoints.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkSparseArray.h"
#include "vtkTable.h"

namespace
{
const std::vector<std::string> arrayNames = { "temperature0", "temperature1", "temperature2",
  "temperature3" };
const std::vector<std::vector<size_t>> resultBins = {
  { 0, 0, 1, 1, 2, 2, 3, 3 },
  { 0, 1, 1, 2, 2, 3, 3, 4 },
  { 0, 1, 2, 2, 3, 4, 4, 5 },
  { 0, 1, 2, 3, 3, 4, 5, 6 },
};
const std::vector<size_t> resultFrequency = { 2, 1, 1, 1, 1, 1, 1, 2 };
const int nData = 10;
const std::vector<size_t> bins = { 4, 5, 6, 7 };

vtkNew<vtkPolyData> MakeTestDataset()
{
  vtkNew<vtkPolyData> dataset;

  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(nData);
  for (int i = 0; i < nData; ++i)
  {
    points->SetPoint(i, i, 0.0, 0.0);
  }
  dataset->SetPoints(points);

  vtkPointData* pd = dataset->GetPointData();
  for (int field = 0; field < 4; ++field)
  {
    vtkNew<vtkDoubleArray> scalars;
    scalars->SetName(arrayNames[field].c_str());
    scalars->SetNumberOfComponents(1);
    scalars->SetNumberOfTuples(nData);
    for (int i = 0; i < nData; ++i)
    {
      scalars->SetTuple1(i, i * (field + 1.0));
    }
    pd->AddArray(scalars);
  }

  return dataset;
}
}

int TestVTKMNDHistogram(int, char*[])
{
  auto ds = MakeTestDataset();

  vtkNew<vtkmNDHistogram> filter;
  filter->SetInputData(ds);
  size_t index = 0;
  for (const auto& an : arrayNames)
  {
    filter->AddFieldAndBin(an, bins[index++]);
  }
  filter->Update();
  vtkArrayData* arrayData = filter->GetOutput();

  assert(arrayData != nullptr);
  // Valid the data range and bin delta
  for (vtkIdType i = 0; i < 4; i++)
  {
    // Validate the delta and range
    auto range = filter->GetDataRange(i);
    double delta = filter->GetBinDelta(i);
    if (range.first != 0.0 || range.second != (i + 1) * 9)
    {
      std::cout << "array index=" << i << " does not have right range" << std::endl;
      return 1;
    }
    if (delta != ((range.second - range.first) / bins[i]))
    {
      std::cout << "array index" << i << " does not have right delta" << std::endl;
      return 1;
    }
  }
  vtkSparseArray<double>* sa = static_cast<vtkSparseArray<double>*>(arrayData->GetArray(0));
  vtkArrayCoordinates coordinates;
  const vtkIdType dimensions = sa->GetDimensions();     // 4
  const vtkIdType non_null_size = sa->GetNonNullSize(); // 8
  for (vtkIdType n = 0; n != non_null_size; ++n)
  {
    sa->GetCoordinatesN(n, coordinates);
    for (vtkIdType d = 0; d != dimensions; ++d)
    {
      assert(coordinates[d] == static_cast<vtkIdType>(resultBins[d][n]));
      if (coordinates[d] != static_cast<vtkIdType>(resultBins[d][n]))
      {
        std::cout << "value does not match at index " << n << " dimension " << d << std::endl;
      }
    }
    assert(resultFrequency[n] == sa->GetValue(coordinates));
  }

  (void)resultFrequency; // Avoid unused-variable warning when NDEBUG is defined
  return 0;
}
