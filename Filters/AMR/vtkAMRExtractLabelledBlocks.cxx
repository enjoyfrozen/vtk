/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAMRExtractLabelledBlocks.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkAMRExtractLabelledBlocks.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataArrayRange.h"
#include "vtkDataSetAttributes.h"
#include "vtkGenericCell.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkSMPThreadLocal.h"
#include "vtkSMPTools.h"
#include "vtkSmartPointer.h"
#include "vtkStructuredData.h"
#include "vtkUnstructuredGrid.h"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

vtkStandardNewMacro(vtkAMRExtractLabelledBlocks);

namespace
{
//===========================================================================
struct PartsRearranger
{
  void operator()(vtkIdType startId, vtkIdType endId)
  {
    const int* dimensions = this->Input->GetDimensions();
    auto& idsPerPart = this->IdsPerPartBlockArray.Local();
    auto& pointIdMapBlock = this->PointIdMap.Local();
    int cellIJK[3];
    auto levelMask = vtk::DataArrayValueRange<1>(this->LevelMask);
    auto partIds = vtk::DataArrayValueRange<1>(this->PartIds);
    for (vtkIdType cellId = startId; cellId < endId; ++cellId)
    {
      if (this->Input->IsCellVisible(cellId) && static_cast<int>(levelMask[cellId]))
      {
        int blockId = partIds[cellId];
        vtkStructuredData::ComputeCellStructuredCoords(cellId, dimensions, cellIJK);
        auto& pointIdMap = pointIdMapBlock[blockId];

        int pointIJK[3];
        for (pointIJK[2] = cellIJK[2]; pointIJK[2] <= cellIJK[2] + (dimensions[2] != 0);
             ++pointIJK[2])
        {
          for (pointIJK[1] = cellIJK[1]; pointIJK[1] <= cellIJK[1] + (dimensions[1] != 0);
               ++pointIJK[1])
          {
            for (pointIJK[0] = cellIJK[0]; pointIJK[0] <= cellIJK[0] + (dimensions[0] != 0);
                 ++pointIJK[0])
            {
              vtkIdType pointId = vtkStructuredData::ComputePointId(dimensions, pointIJK);

              // We allocate pointIdMap on pointId
              pointIdMap[pointId];
            }
          }
        }
        idsPerPart[blockId]->InsertNextId(cellId);
      }
    }
  }

  void Initialize()
  {
    this->IdsPerPartBlockArray.Local().resize(this->NumberOfBlocks);
    for (auto& item : this->IdsPerPartBlockArray.Local())
    {
      item = vtkSmartPointer<vtkIdList>::New();
    }
    this->PointIdMap.Local().resize(this->NumberOfBlocks);
  }

  void Reduce() {}

  vtkImageData* Input;
  vtkDataArray* PartIds;
  vtkDataArray* LevelMask;
  int NumberOfBlocks;

  vtkSMPThreadLocal<std::vector<std::map<vtkIdType, vtkIdType>>> PointIdMap;
  vtkSMPThreadLocal<std::vector<vtkSmartPointer<vtkIdList>>> IdsPerPartBlockArray;
};

//============================================================================
struct PointIdMapMerger
{
  PointIdMapMerger(int numberOfBlocks, int numberOfInputs)
    : NumberOfBlocks(numberOfBlocks)
  {
    this->PointIdMapSplitPerThread.resize(numberOfInputs);
    for (auto& item : this->PointIdMapSplitPerThread)
    {
      item.resize(numberOfBlocks);
    }

    this->PointIdMapThreadUnion.resize(numberOfBlocks);
    for (auto& item : this->PointIdMapThreadUnion)
    {
      item.resize(numberOfInputs);
    }
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    for (vtkIdType id = startId; id < endId; ++id)
    {
      int blockId = id % this->NumberOfBlocks;
      int inputId = id / this->NumberOfBlocks;
      auto& localPointIdMapThreadUnion = this->PointIdMapThreadUnion[blockId][inputId];
      for (auto& pointIdMapAtThread : this->PointIdMapSplitPerThread[inputId][blockId])
      {
        localPointIdMapThreadUnion.insert(pointIdMapAtThread.begin(), pointIdMapAtThread.end());
      }
    }
  }

  int NumberOfBlocks;

  std::vector<std::vector<std::map<vtkIdType, vtkIdType>>> PointIdMapThreadUnion;
  std::vector<std::vector<std::vector<std::map<vtkIdType, vtkIdType>>>> PointIdMapSplitPerThread;
};

//============================================================================
struct OutputPointsGenerator
{
  OutputPointsGenerator(std::vector<vtkImageData*>& inputs,
    std::vector<std::vector<std::map<vtkIdType, vtkIdType>>>& pointIdMap, int numberOfBlocks)
    : Inputs(inputs)
    , PointIdMap(pointIdMap)
    , NumberOfBlocks(numberOfBlocks)
  {
    this->Points.resize(numberOfBlocks);
    for (vtkPoints* points : this->Points)
    {
      points->SetDataType(VTK_DOUBLE);
    }
    this->PointIdOffset.resize(numberOfBlocks);
    for (int blockId = 0; blockId < numberOfBlocks; ++blockId)
    {
      this->PointIdOffset[blockId].resize(this->Inputs.size());
      vtkIdType pointIdOffset = 0;
      for (int inputId = 0; inputId < static_cast<int>(this->Inputs.size()); ++inputId)
      {
        this->PointIdOffset[blockId][inputId] = pointIdOffset;
        pointIdOffset += this->PointIdMap[blockId][inputId].size();
      }

      this->Points[blockId]->SetNumberOfPoints(pointIdOffset);
    }
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    double point[3];
    for (vtkIdType id = startId; id < endId; ++id)
    {
      int blockId = id % this->NumberOfBlocks;
      int inputId = id / this->NumberOfBlocks;

      auto pointsRange = vtk::DataArrayTupleRange<3>(this->Points[blockId]->GetData());
      vtkImageData* input = this->Inputs[inputId];

      vtkIdType pointIdOffset = this->PointIdOffset[blockId][inputId];
      auto& pointIdMap = this->PointIdMap[blockId][inputId];

      vtkIdType currentPointId = pointIdOffset;
      for (auto& pair : pointIdMap)
      {
        pair.second = currentPointId++;
        input->GetPoint(pair.first, point);
        vtkMath::Assign(point, pointsRange[pair.second]);
      }
    }
  }

  std::vector<vtkImageData*>& Inputs;

  std::vector<std::vector<std::map<vtkIdType, vtkIdType>>>& PointIdMap;
  int NumberOfBlocks;
  std::vector<std::vector<vtkIdType>> PointIdOffset;
  std::vector<vtkNew<vtkPoints>> Points;
};

//============================================================================
struct MultiBlockBuilder
{
  MultiBlockBuilder(std::vector<vtkImageData*>& inputs, std::vector<vtkUnstructuredGrid*>& outputs)
    : Inputs(inputs)
    , MultiBlock(outputs)
  {
    this->Ids.resize(inputs.size());
    this->PointIdMap.resize(outputs.size());
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    vtkNew<vtkGenericCell> genericCell;
    for (vtkIdType blockId = startId; blockId < endId; ++blockId)
    {
      vtkUnstructuredGrid* ug = this->MultiBlock[blockId];

      vtkIdType numberOfCells = 0;
      for (int inputId = 0; inputId < static_cast<int>(this->Inputs.size()); ++inputId)
      {
        for (int threadId = 0; threadId < static_cast<int>(this->Ids[inputId].size()); ++threadId)
        {
          numberOfCells += this->Ids[inputId][threadId][blockId]->GetNumberOfIds();
        }
      }

      vtkCellData* ugCD = ug->GetCellData();

      // We have to do this hacky maneuvre because `vtkFieldData::InsertTuple`
      // doesn't skip arrays set using CopyFieldOff
      vtkNew<vtkCellData> tmpCD;
      tmpCD->ShallowCopy(this->Inputs[0]->GetCellData());
      tmpCD->RemoveArray(vtkDataSetAttributes::GhostArrayName());
      ugCD->CopyStructure(tmpCD);
      ugCD->SetNumberOfTuples(numberOfCells);
      vtkIdType cellIdOffset = 0;

      for (int inputId = 0; inputId < static_cast<int>(this->Ids.size()); ++inputId)
      {
        for (int threadId = 0; threadId < static_cast<int>(this->Ids[inputId].size()); ++threadId)
        {
          vtkImageData* input = this->Inputs[inputId];
          vtkCellData* inputCD = input->GetCellData();
          tmpCD->ShallowCopy(inputCD);
          tmpCD->RemoveArray(vtkDataSetAttributes::GhostArrayName());
          vtkIdList* ids = this->Ids[inputId][threadId][blockId];
          const auto& pointIdMap = PointIdMap[blockId][inputId];

          for (vtkIdType id = 0; id < ids->GetNumberOfIds(); ++id)
          {
            vtkIdType inputCellId = ids->GetId(id);

            ugCD->InsertTuple(cellIdOffset + id, inputCellId, tmpCD);

            input->GetCell(inputCellId, genericCell);
            vtkIdList* cellPointIds = genericCell->GetPointIds();
            vtkIdType* cellPointIdsPointer = cellPointIds->GetPointer(0);
            for (vtkIdType pointId = 0; pointId < cellPointIds->GetNumberOfIds(); ++pointId)
            {
              cellPointIdsPointer[pointId] = pointIdMap.at(cellPointIdsPointer[pointId]);
            }
            ug->InsertNextCell(
              genericCell->GetCellType(), genericCell->GetNumberOfPoints(), cellPointIdsPointer);
          }

          cellIdOffset += ids->GetNumberOfIds();
        }
      }
    }
  }

  std::vector<vtkImageData*>& Inputs;
  std::vector<vtkUnstructuredGrid*>& MultiBlock;
  std::vector<std::vector<std::vector<vtkSmartPointer<vtkIdList>>>> Ids;
  std::vector<std::vector<std::map<vtkIdType, vtkIdType>>> PointIdMap;
};
} // anonymous namespace

//----------------------------------------------------------------------------
vtkAMRExtractLabelledBlocks::vtkAMRExtractLabelledBlocks() {}

//----------------------------------------------------------------------------
vtkAMRExtractLabelledBlocks::~vtkAMRExtractLabelledBlocks() {}

//------------------------------------------------------------------------------
int vtkAMRExtractLabelledBlocks::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUniformGridAMR");
  return 1;
}

//----------------------------------------------------------------------------
int vtkAMRExtractLabelledBlocks::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkDataObject* inputDO = vtkDataObject::GetData(inputVector[0], 0);
  vtkDataObject* outputDO = vtkDataObject::GetData(outputVector, 0);

  std::vector<vtkImageData*> inputsID = vtkCompositeDataSet::GetDataSets<vtkImageData>(inputDO);

  // Computing the number of output blocks we need to generate by looking at the range of PartId
  int maxPartId = 0;
  for (auto& input : inputsID)
  {
    maxPartId = std::max<int>(
      maxPartId, input->GetCellData()->GetArray(this->PartIdArrayName)->GetRange()[1]);
  }

  int numberOfBlocks = maxPartId + 1;

  vtkMultiBlockDataSet* outputMBDS = vtkMultiBlockDataSet::SafeDownCast(outputDO);
  outputMBDS->SetNumberOfBlocks(numberOfBlocks);

  for (int blockId = 0; blockId < numberOfBlocks; ++blockId)
  {
    outputMBDS->SetBlock(blockId, vtkNew<vtkUnstructuredGrid>());
  }

  std::vector<vtkUnstructuredGrid*> outputsUG =
    vtkCompositeDataSet::GetDataSets<vtkUnstructuredGrid>(outputMBDS);

  // This builder will generate the output cell geometry.
  MultiBlockBuilder builder(inputsID, outputsUG);

  // This merger will merge points from different threads that have the same point id
  // inside the same input uniform grid.
  PointIdMapMerger pointIdMapMerger(numberOfBlocks, inputsID.size());

  for (int imageId = 0; imageId < static_cast<int>(inputsID.size()); ++imageId)
  {
    // For each input uniform grid, we compute the point id map / cell id mapping
    // them to their output block.
    PartsRearranger rearranger;
    vtkImageData* input = inputsID[imageId];
    vtkCellData* inputCD = input->GetCellData();
    rearranger.PartIds = inputCD->GetArray(this->PartIdArrayName);
    rearranger.LevelMask = inputCD->GetArray(this->LevelMaskArrayName);
    rearranger.NumberOfBlocks = numberOfBlocks;
    rearranger.Input = input;

    if (!rearranger.LevelMask || !rearranger.PartIds)
    {
      vtkErrorMacro("You forgot to set PartIdArrayName and LevelMaskArrayName... "
        << " The filter won't produce anything");
      return 0;
    }

    vtkSMPTools::For(0, input->GetNumberOfCells(), rearranger);

    // Here, we move data from the rearranger to the builder or the point id map merger.
    auto idsIt = rearranger.IdsPerPartBlockArray.begin();
    auto pointIdMapIt = rearranger.PointIdMap.begin();
    for (; idsIt != rearranger.IdsPerPartBlockArray.end(); ++idsIt, ++pointIdMapIt)
    {
      builder.Ids[imageId].emplace_back(std::move((*idsIt)));
      for (int blockId = 0; blockId < numberOfBlocks; ++blockId)
      {
        pointIdMapMerger.PointIdMapSplitPerThread[imageId][blockId].emplace_back(
          std::move((*pointIdMapIt)[blockId]));
      }
    }
  }

  // We iterate on each input grid and each output block and merge the point id maps.
  vtkSMPTools::For(0, inputsID.size() * numberOfBlocks, pointIdMapMerger);

  builder.PointIdMap = std::move(pointIdMapMerger.PointIdMapThreadUnion);

  // We now generate the output point array using the point id map that we just merged.
  OutputPointsGenerator pointsGenerator(inputsID, builder.PointIdMap, numberOfBlocks);
  vtkSMPTools::For(0, inputsID.size() * numberOfBlocks, pointsGenerator);

  for (int blockId = 0; blockId < numberOfBlocks; ++blockId)
  {
    outputsUG[blockId]->SetPoints(pointsGenerator.Points[blockId]);
  }

  // Finally, we generate the geometry of each block.
  vtkSMPTools::For(0, numberOfBlocks, builder);

  return 1;
}

//----------------------------------------------------------------------------
void vtkAMRExtractLabelledBlocks::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
