/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestExtractGhostCells.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkExtractGhostCells.h"

#include "vtkCellCenters.h"
#include "vtkCellData.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkImageData.h"
#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPointData.h"
#include "vtkPointDataToCellData.h"
#include "vtkRTAnalyticSource.h"
#include "vtkStaticPointLocator.h"
#include "vtkStructuredData.h"
#include "vtkStructuredGrid.h"
#include "vtkUnsignedCharArray.h"

//----------------------------------------------------------------------------
bool TestValues(vtkDataSet* ref, vtkStaticPointLocator* locator, vtkDataSet* ds)
{
  vtkDataArray* refArray = ref->GetPointData()->GetArray("RTData");
  vtkDataArray* array = ds->GetPointData()->GetArray("RTData");

  for (vtkIdType pointId = 0; pointId < ds->GetNumberOfPoints(); ++pointId)
  {
    double* p = ds->GetPoint(pointId);
    vtkIdType refPointId = locator->FindClosestPoint(p);

    if (refArray->GetTuple1(refPointId) != array->GetTuple1(pointId))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
int TestExtractGhostCells(int, char*[])
{
  int retVal = EXIT_SUCCESS;

  vtkNew<vtkRTAnalyticSource> image1;
  image1->SetWholeExtent(-5, 0, -5, 5, -5, 5);

  vtkNew<vtkPointDataToCellData> point2cell1;
  point2cell1->SetInputConnection(image1->GetOutputPort());
  point2cell1->Update();

  vtkNew<vtkRTAnalyticSource> image2;
  image2->SetWholeExtent(0, 5, -5, 5, -5, 5);
  image2->Update();

  vtkNew<vtkPointDataToCellData> point2cell2;
  point2cell2->SetInputConnection(image2->GetOutputPort());
  point2cell2->Update();

  vtkNew<vtkUnsignedCharArray> ghosts1, ghosts2;

  vtkNew<vtkImageData> im1, im2;

  im1->ShallowCopy(point2cell1->GetOutputDataObject(0));
  im2->ShallowCopy(point2cell2->GetOutputDataObject(0));

  ghosts1->SetNumberOfValues(im1->GetNumberOfCells());
  ghosts2->SetNumberOfValues(im2->GetNumberOfCells());

  ghosts1->SetName(vtkDataSetAttributes::GhostArrayName());
  ghosts2->SetName(vtkDataSetAttributes::GhostArrayName());

  int kmin = 0, kmax = 4, jmin = 0, jmax = 4, imin = 0, imax = 3;
  int dims[3] = { 5, 10, 10 };
  int ijk[3];

  for (ijk[2] = kmin; ijk[2] < kmax; ++ijk[2])
  {
    for (ijk[1] = jmin; ijk[1] < jmax; ++ijk[1])
    {
      for (ijk[0] = imin; ijk[0] < imax; ++ijk[0])
      {
        vtkIdType cellId = vtkStructuredData::ComputeCellId(ijk, dims);
        ghosts1->SetValue(cellId, vtkDataSetAttributes::CellGhostTypes::DUPLICATECELL);
        ghosts2->SetValue(cellId, vtkDataSetAttributes::CellGhostTypes::DUPLICATECELL);
      }
    }
  }

  im1->GetCellData()->AddArray(ghosts1);
  im2->GetCellData()->AddArray(ghosts1);

  vtkNew<vtkCellCenters> centers1;
  centers1->SetInputData(im1);
  centers1->Update();
  auto points1 = vtkDataSet::SafeDownCast(centers1->GetOutputDataObject(0));

  vtkNew<vtkStaticPointLocator> locator1;
  locator1->SetDataSet(points1);
  locator1->BuildLocator();

  vtkNew<vtkCellCenters> centers2;
  centers2->SetInputData(im2);
  centers2->Update();
  auto points2 = vtkDataSet::SafeDownCast(centers2->GetOutputDataObject(0));

  vtkNew<vtkStaticPointLocator> locator2;
  locator2->SetDataSet(points2);
  locator2->BuildLocator();

  vtkNew<vtkPartitionedDataSet> pds;
  pds->SetNumberOfPartitions(2);
  pds->SetPartition(0, im1);
  pds->SetPartition(1, im2);
  centers1->Update();

  // Testing filter for composite data set

  vtkNew<vtkExtractGhostCells> extract1;
  extract1->SetInputData(pds);
  extract1->Update();

  auto outPDS = vtkPartitionedDataSet::SafeDownCast(extract1->GetOutputDataObject(0));

  vtkNew<vtkCellCenters> outPDSCenters1;
  outPDSCenters1->SetInputData(outPDS->GetPartition(0));
  outPDSCenters1->Update();
  auto outPDSPoints1 = vtkDataSet::SafeDownCast(outPDSCenters1->GetOutputDataObject(0));

  vtkNew<vtkCellCenters> outPDSCenters2;
  outPDSCenters2->SetInputData(outPDS->GetPartition(1));
  outPDSCenters2->Update();
  auto outPDSPoints2 = vtkDataSet::SafeDownCast(outPDSCenters2->GetOutputDataObject(0));

  if (!TestValues(points1, locator1, outPDSPoints1) ||
    !TestValues(points2, locator2, outPDSPoints2))
  {
    vtkLog(ERROR, "Extracting ghost cells failed for partitioned data set.");
    retVal = EXIT_FAILURE;
  }

  // Testing filter for data set

  vtkNew<vtkExtractGhostCells> extract2;
  extract2->SetInputData(im1);
  extract2->Update();

  vtkNew<vtkCellCenters> outCenters;
  outCenters->SetInputData(outPDS->GetPartition(0));
  outCenters->Update();
  auto outPoints = vtkDataSet::SafeDownCast(outCenters->GetOutputDataObject(0));

  if (!TestValues(points1, locator1, outPoints))
  {
    vtkLog(ERROR, "Extracting ghost cells failed for data set.");
    retVal = EXIT_FAILURE;
  }

  return retVal;
}
