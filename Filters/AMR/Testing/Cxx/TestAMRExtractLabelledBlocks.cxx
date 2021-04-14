/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestAMRExtractLabelledBlocks.cxx

  Copyright (c) Kitware SAS
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME TestAMRExtractLabelledBlocks.cxx -- Regression test for AMR Ghost Zones
//
// .SECTION Description
//  Test that computing ghost zones does not erase previous ghost zones.

#include "vtkAMRExtractLabelledBlocks.h"
#include "vtkCellData.h"
#include "vtkCompositeDataSet.h"
#include "vtkIntArray.h"
#include "vtkLogger.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkOverlappingAMR.h"
#include "vtkSmartPointer.h"
#include "vtkStructuredData.h"
#include "vtkUniformGrid.h"
#include "vtkUnstructuredGrid.h"

namespace
{
constexpr double Spacing0[3] = { 2.0, 2.0, 2.0 };
constexpr double Spacing1[3] = { 1.0, 1.0, 1.0 };
constexpr int BlocksPerLevel[] = { 2, 1 };
constexpr int WidthLevel0 = 30;
constexpr int WidthLevel1 = 30;
constexpr int TubeWidth = 10;
constexpr char PartIdArrayName[] = "part_id";
constexpr char LevelMaskArrayName[] = "level_mask";

//------------------------------------------------------------------------------
vtkSmartPointer<vtkOverlappingAMR> CreateAMR()
{
  vtkSmartPointer<vtkOverlappingAMR> amr = vtkSmartPointer<vtkOverlappingAMR>::New();

  amr->Initialize(2, BlocksPerLevel);

  vtkNew<vtkUniformGrid> image1;
  int extent1[6] = { -WidthLevel0, 0, -WidthLevel0, WidthLevel0, -WidthLevel0, WidthLevel0 };
  image1->SetExtent(extent1);
  image1->SetSpacing(Spacing0);

  vtkNew<vtkIntArray> partId1;
  partId1->SetNumberOfComponents(1);
  partId1->SetNumberOfTuples(image1->GetNumberOfCells());
  partId1->SetName(PartIdArrayName);
  vtkNew<vtkIntArray> levelMask1;
  levelMask1->SetNumberOfComponents(1);
  levelMask1->SetNumberOfTuples(image1->GetNumberOfCells());
  levelMask1->SetName(LevelMaskArrayName);

  int ijk[3];
  for (ijk[2] = extent1[4]; ijk[2] < extent1[5]; ++ijk[2])
  {
    for (ijk[1] = extent1[2]; ijk[1] < extent1[3]; ++ijk[1])
    {
      for (ijk[0] = extent1[0]; ijk[0] < extent1[1]; ++ijk[0])
      {
        vtkIdType cellId = vtkStructuredData::ComputeCellIdForExtent(extent1, ijk);
        partId1->SetValue(cellId, std::abs(ijk[0]) > TubeWidth || std::abs(ijk[1]) > TubeWidth);
        levelMask1->SetValue(cellId,
          2 * std::abs(ijk[0]) < WidthLevel1 || 2 * std::abs(ijk[1]) < WidthLevel1 ||
            2 * std::abs(ijk[2]) < WidthLevel1);
      }
    }
  }
  vtkCellData* image1CD = image1->GetCellData();
  image1CD->AddArray(partId1);
  image1CD->AddArray(levelMask1);

  amr->SetDataSet(0, 0, image1);

  vtkNew<vtkUniformGrid> image2;
  int extent2[6] = { 0, WidthLevel0, -WidthLevel0, WidthLevel0, -WidthLevel0, WidthLevel0 };
  image2->SetExtent(extent2);
  image2->SetSpacing(Spacing0);

  vtkNew<vtkIntArray> partId2;
  partId2->SetNumberOfComponents(1);
  partId2->SetNumberOfTuples(image2->GetNumberOfCells());
  partId2->SetName(PartIdArrayName);
  vtkNew<vtkIntArray> levelMask2;
  levelMask2->SetNumberOfComponents(1);
  levelMask2->SetNumberOfTuples(image2->GetNumberOfCells());
  levelMask2->SetName(LevelMaskArrayName);

  for (ijk[2] = extent2[4]; ijk[2] < extent2[5]; ++ijk[2])
  {
    for (ijk[1] = extent2[2]; ijk[1] < extent2[3]; ++ijk[1])
    {
      for (ijk[0] = extent2[0]; ijk[0] < extent2[1]; ++ijk[0])
      {
        vtkIdType cellId = vtkStructuredData::ComputeCellIdForExtent(extent2, ijk);
        partId2->SetValue(cellId, std::abs(ijk[0]) < TubeWidth || std::abs(ijk[1]) < TubeWidth);
        levelMask2->SetValue(cellId,
          2 * std::abs(ijk[0]) < WidthLevel1 || 2 * std::abs(ijk[1]) < WidthLevel1 ||
            2 * std::abs(ijk[2]) < WidthLevel1);
      }
    }
  }

  vtkCellData* image2CD = image2->GetCellData();
  image2CD->AddArray(partId2);
  image2CD->AddArray(levelMask2);

  amr->SetDataSet(0, 1, image2);

  vtkNew<vtkUniformGrid> image3;
  int extent3[6] = { -WidthLevel1, WidthLevel1, -WidthLevel1, WidthLevel1, -WidthLevel1,
    WidthLevel1 };
  image3->SetExtent(extent3);
  image3->SetSpacing(Spacing1);

  vtkNew<vtkIntArray> partId3;
  partId3->SetNumberOfComponents(1);
  partId3->SetNumberOfTuples(image3->GetNumberOfCells());
  partId3->SetName(PartIdArrayName);
  vtkNew<vtkIntArray> levelMask3;
  levelMask3->SetNumberOfComponents(1);
  levelMask3->SetNumberOfTuples(image3->GetNumberOfCells());
  levelMask3->SetName(LevelMaskArrayName);

  for (ijk[2] = extent3[4]; ijk[2] < extent3[5]; ++ijk[2])
  {
    for (ijk[1] = extent3[2]; ijk[1] < extent3[3]; ++ijk[1])
    {
      for (ijk[0] = extent3[0]; ijk[0] < extent3[1]; ++ijk[0])
      {
        vtkIdType cellId = vtkStructuredData::ComputeCellIdForExtent(extent3, ijk);
        partId3->SetValue(
          cellId, 2 * std::abs(ijk[0]) < TubeWidth || 2 * std::abs(ijk[1]) < TubeWidth);
        levelMask3->SetValue(cellId, 1);
      }
    }
  }

  vtkCellData* image3CD = image3->GetCellData();
  image3CD->AddArray(partId3);
  image3CD->AddArray(levelMask3);

  amr->SetDataSet(1, 0, image3);

  return amr;
}

//------------------------------------------------------------------------------
bool OutputIsCorrect(vtkMultiBlockDataSet* mbds)
{
  std::vector<vtkUnstructuredGrid*> ugs =
    vtkCompositeDataSet::GetDataSets<vtkUnstructuredGrid>(mbds);

  if (ugs.size() != 2)
  {
    vtkLog(ERROR, "There should be 2 output blocks instead of " << ugs.size());
    return false;
  }

  for (vtkUnstructuredGrid* ug : ugs)
  {
    vtkCellData* ugCD = ug->GetCellData();
    vtkIntArray* partId = vtkArrayDownCast<vtkIntArray>(ugCD->GetAbstractArray(PartIdArrayName));
    vtkIntArray* levelMask =
      vtkArrayDownCast<vtkIntArray>(ugCD->GetAbstractArray(LevelMaskArrayName));

    int partIdVal = partId->GetValue(0);
    for (vtkIdType cellId = 1; cellId < ug->GetNumberOfCells(); ++cellId)
    {
      if (!levelMask->GetValue(cellId) || partId->GetValue(cellId) != partIdVal)
      {
        vtkLog(ERROR, "Generated output is faultly");
        return false;
      }
    }
  }
  return true;
}
} // anonymous namespace

//------------------------------------------------------------------------------
int TestAMRExtractLabelledBlocks(int, char**)
{
  vtkSmartPointer<vtkOverlappingAMR> amr = CreateAMR();

  vtkNew<vtkAMRExtractLabelledBlocks> extractor;
  extractor->SetInputData(amr);
  extractor->SetPartIdArrayName(PartIdArrayName);
  extractor->SetLevelMaskArrayName(LevelMaskArrayName);
  extractor->Update();

  return OutputIsCorrect(vtkMultiBlockDataSet::SafeDownCast(extractor->GetOutputDataObject(0)))
    ? EXIT_SUCCESS
    : EXIT_FAILURE;
}
