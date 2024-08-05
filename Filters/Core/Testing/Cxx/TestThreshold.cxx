// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkCellData.h"
#include "vtkDataObject.h"
#include "vtkDataSetAttributes.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkRTAnalyticSource.h"
#include "vtkSmartPointer.h"
#include "vtkStructuredData.h"
#include "vtkThreshold.h"
#include "vtkType.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"
#include <vtkAffineArray.h>

#include <cmath>
#include <limits>

class MockDataArray : public vtkAffineArray<double>
{
public:
  vtkTypeMacro(MockDataArray, vtkAffineArray<double>);
  static MockDataArray* New();

  void* GetVoidPointer(vtkIdType idx) override
  {
    vtkLogF(ERROR, "Accessed Array::GetVoidPointer");
    return vtkAffineArray<double>::GetVoidPointer(idx);
  }
};

vtkStandardNewMacro(MockDataArray);

class MockLargeImageData : public vtkImageData
{
public:
  vtkTypeMacro(MockLargeImageData, vtkImageData);
  static MockLargeImageData* New();

  /**
   * @brief Initialize with dimensions large enough for numberOfCells and numberOfPoints > INT_MAX
   */
  MockLargeImageData()
  {
    vtkIdType numericLimit = std::numeric_limits<int>::max();
    vtkIdType desiredNumberOfCells = numericLimit + 1;
    vtkIdType desiredNumberOfPoints = (desiredNumberOfCells * 2) + 1;
    vtkIdType dim = std::cbrt(desiredNumberOfPoints);

    assert(dim < numericLimit);

    this->SetDimensions(static_cast<int>(dim), static_cast<int>(dim), static_cast<int>(dim));

    assert(this->GetNumberOfCells() > numericLimit);
    assert(this->GetNumberOfPoints() > numericLimit);
  }

  /**
   * @brief Add an implicit Point Scalar array
   */
  void GeneratePointScalarData()
  {
    vtkNew<MockDataArray> array;
    array->ConstructBackend(1, 6); // Make random ?
    array->SetNumberOfComponents(1);
    array->SetNumberOfTuples(this->GetNumberOfPoints());
    array->SetName("Points Scalars");
    this->GetPointData()->SetScalars(array);
  }

  void GeneratePointData()
  {
    vtkNew<MockDataArray> array;
    array->ConstructBackend(1, 6);   // Make random ?
    array->SetNumberOfComponents(3); // Parametric ?
    array->SetNumberOfTuples(this->GetNumberOfPoints());
    array->SetName("Point Data"); // Parametric ?
    this->GetPointData()->AddArray(array);
  }

  void GenerateCellData()
  {
    vtkNew<MockDataArray> array;
    array->ConstructBackend(1, 6);   // Make random ?
    array->SetNumberOfComponents(3); // Parametric ?
    array->SetNumberOfTuples(this->GetNumberOfCells());
    array->SetName("Point Data"); // Parametric ?
    this->GetCellData()->AddArray(array);
  }
};

vtkStandardNewMacro(MockLargeImageData);

int TestThreshold(int, char*[])
{
  // Test large imagedata
  vtkNew<MockLargeImageData> image;
  image->GeneratePointScalarData();

  vtkNew<vtkThreshold> threshold;
  threshold->SetInputData(image);

  threshold->Update();

  //---------------------------------------------------
  // Test using different thresholding methods
  //---------------------------------------------------
  vtkNew<vtkRTAnalyticSource> source;

  // We're setting a ghost array with one hidden ghost cell
  // This cell should disappear after thresholding
  source->Update();
  vtkIdType numberOfHiddenCells = 1;

  vtkNew<vtkImageData> ghostedWavelet;
  ghostedWavelet->ShallowCopy(source->GetOutputDataObject(0));

  vtkNew<vtkUnsignedCharArray> ghosts;
  ghosts->SetName(vtkDataSetAttributes::GhostArrayName());
  ghosts->SetNumberOfValues(ghostedWavelet->GetNumberOfCells());
  ghosts->Fill(0);

  ghosts->SetValue(19, vtkDataSetAttributes::HIDDENCELL);

  ghostedWavelet->GetCellData()->AddArray(ghosts);

  vtkNew<vtkThreshold> filter;
  filter->SetInputData(ghostedWavelet);

  double L = 100.0;
  double U = 200.0;
  filter->SetThresholdFunction(vtkThreshold::THRESHOLD_BETWEEN);
  filter->SetLowerThreshold(L);
  filter->SetUpperThreshold(U);
  filter->SetAllScalars(0);
  filter->Update();
  int n1 = filter->GetOutput()->GetNumberOfCells();

  filter->UseContinuousCellRangeOn();
  filter->Update();
  int n2 = filter->GetOutput()->GetNumberOfCells();

  // we are using a large query range,
  // whether to use continuous range or not should not matter
  if (n1 != n2)
  {
    return EXIT_FAILURE;
  }

  filter->UseContinuousCellRangeOff();
  filter->SetUpperThreshold(L);
  filter->Update();
  // since we are not using continuous cell range
  // no cell points should fall in the empty interval
  if (filter->GetOutput()->GetNumberOfCells() > 0)
  {
    return EXIT_FAILURE;
  }
  filter->UseContinuousCellRangeOn();
  filter->Update();
  if (filter->GetOutput()->GetNumberOfCells() == 0)
  {
    return EXIT_FAILURE;
  }

  // Get the total number of cells
  int totalCellCount = source->GetOutput()->GetNumberOfCells();
  int thresholdedCellCount = filter->GetOutput()->GetNumberOfCells();

  // Now invert the threshold and test the number of cells
  filter->InvertOn();
  filter->Update();
  int invertedCellCount = filter->GetOutput()->GetNumberOfCells();
  if (invertedCellCount + thresholdedCellCount != totalCellCount - numberOfHiddenCells)
  {
    std::cerr << "Cell count and inverted cell count inconsistent" << std::endl;
    return EXIT_FAILURE;
  }

  // Revert attributes to default values
  filter->AllScalarsOn();
  filter->InvertOff();
  filter->UseContinuousCellRangeOff();

  // Check the number of cells after thresholding below
  filter->SetThresholdFunction(vtkThreshold::THRESHOLD_LOWER);
  filter->SetLowerThreshold(L);
  filter->Update();
  if (filter->GetOutput()->GetNumberOfCells() != 131)
  {
    std::cerr << "Unexpected cell count after thresholding below" << std::endl;
    return EXIT_FAILURE;
  }

  // Check the number of cells after thresholding above
  filter->SetThresholdFunction(vtkThreshold::THRESHOLD_UPPER);
  filter->SetUpperThreshold(U);
  filter->Update();
  if (filter->GetOutput()->GetNumberOfCells() != 780)
  {
    std::cerr << "Unexpected cell count after thresholding above" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
