// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkRadiossAnimReader.h"

#include "RadiossAnimDataModel.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkUnstructuredGrid.h"

#include <set>
#include <vtksys/SystemTools.hxx>

VTK_ABI_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkRadiossAnimReader);

//------------------------------------------------------------------------------
vtkRadiossAnimReader::vtkRadiossAnimReader()
{
  this->SetNumberOfInputPorts(0);
}

//------------------------------------------------------------------------------
vtkRadiossAnimReader::~vtkRadiossAnimReader() = default;

//------------------------------------------------------------------------------
void vtkRadiossAnimReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << "FileName: " << (this->FileName.empty() ? "Empty" : this->FileName) << std::endl;
}

//------------------------------------------------------------------------------
int vtkRadiossAnimReader::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  if (this->FileName.empty())
  {
    vtkErrorMacro("RequestInformation called for RadiossAnimReader without file name");
    return 0;
  }

  try
  {
    this->RadiossAnimDataModel =
      std::unique_ptr<class RadiossAnimDataModel>(new class RadiossAnimDataModel(this->FileName));
  }
  catch (const std::runtime_error& exception)
  {
    vtkErrorMacro("Exception raised while reading the file: "
      << this->FileName << "\nException message: " << exception.what());
    return 0;
  }

  return 1;
}

// ----------------------------------------------------------------------------
int vtkRadiossAnimReader::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkPartitionedDataSetCollection* output = vtkPartitionedDataSetCollection::GetData(outInfo);

  if (!output)
  {
    vtkErrorMacro("Unable to retrieve the output!");
    return 0;
  }

  float time = this->RadiossAnimDataModel->GetTime();
  auto& radiossNodes = this->RadiossAnimDataModel->GetNodes();
  auto& radiossQuads = this->RadiossAnimDataModel->GetQuads();

  output->SetNumberOfPartitionedDataSets(radiossQuads.Parts.size());
  for (size_t partIndex = 0; partIndex < radiossQuads.Parts.size(); ++partIndex)
  {
    auto& part = radiossQuads.Parts[partIndex];
    std::unordered_map<int, vtkIdType> radiossPointIdToVTKPointIndex;
    vtkNew<vtkPoints> points;
    vtkNew<vtkIntArray> radiossNodeIDs;
    const int numberOfPointsPerCell = 4;
    ExtractPartPoints(part.FirstCellIndex, part.LastCellIndex, radiossQuads.Connectivity,
      radiossNodes.Coordinates, numberOfPointsPerCell, points, radiossNodeIDs,
      radiossPointIdToVTKPointIndex);

    auto unstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    unstructuredGrid->SetPoints(points);
    unstructuredGrid->GetPointData()->AddArray(radiossNodeIDs);

    // Add Cells
    int numberOfCells = part.LastCellIndex - part.FirstCellIndex + 1;
    unstructuredGrid->Allocate(numberOfCells);
    for (int cellIndex = part.FirstCellIndex; cellIndex <= part.LastCellIndex; ++cellIndex)
    {
      std::set<int> pointIDs;
      for (int i = 0; i < numberOfPointsPerCell; i++)
      {
        pointIDs.insert(radiossQuads.Connectivity[(numberOfPointsPerCell * cellIndex) + i]);
      }
      std::vector<vtkIdType> vtkPointIDs;
      VTKCellType cellType = VTK_QUAD;
      if (pointIDs.size() == 1)
      {
        // Vertex case.
        cellType = VTK_VERTEX;
        for (auto pointID : pointIDs)
        {
          vtkPointIDs.push_back(radiossPointIdToVTKPointIndex[pointID]);
        }
      }
      else if (pointIDs.size() == 2)
      {
        // Line case.
        cellType = VTK_LINE;
        for (auto pointID : pointIDs)
        {
          vtkPointIDs.push_back(radiossPointIdToVTKPointIndex[pointID]);
        }
      }
      else if (pointIDs.size() == 3)
      {
        // Triangle case.
        cellType = VTK_TRIANGLE;
        for (auto pointID : pointIDs)
        {
          vtkPointIDs.push_back(radiossPointIdToVTKPointIndex[pointID]);
        }
      }
      else
      {
        if (pointIDs.size() != 4)
        {
          vtkWarningWithObjectMacro(nullptr,
            "Cell (" << cellIndex << ") with " << pointIDs.size() << " nodes represented as Quad.");
        }
        // General case with Quad.
        for (int i = 0; i < numberOfPointsPerCell; i++)
        {
          int pointID = radiossQuads.Connectivity[(numberOfPointsPerCell * cellIndex) + i];
          vtkPointIDs.push_back(radiossPointIdToVTKPointIndex[pointID]);
        }
      }
      unstructuredGrid->InsertNextCell(cellType, vtkPointIDs.size(), vtkPointIDs.data());
    }

    // Node Point Data
    std::vector<vtkSmartPointer<vtkAbstractArray>> nodePointDatas;
    ExtractPartPointData(radiossQuads, radiossNodes, radiossNodeIDs, nodePointDatas);
    for (auto& nodePointData : nodePointDatas)
    {
      unstructuredGrid->GetPointData()->AddArray(nodePointData);
    }

    // Add to the Partitioned DataSet collection.
    vtkNew<vtkPartitionedDataSet> partitionedDataSet;
    partitionedDataSet->SetNumberOfPartitions(1);
    partitionedDataSet->SetPartition(0, unstructuredGrid);
    output->SetPartitionedDataSet(partIndex, partitionedDataSet);
  }

  return 1;
}

void vtkRadiossAnimReader::ExtractPartPoints(int minCellIndex, int maxCellIndex,
  const std::vector<int>& radiossCellConnectivity,
  const std::vector<float>& radiossPointCoordinates, int numberOfPointsPerCell, vtkPoints* points,
  vtkIntArray* radiossNodeIDs, std::unordered_map<int, vtkIdType>& radiossPointIdToVTKPointIndex)
{
  std::set<int> partPointIDs;
  for (int cellIndex = minCellIndex; cellIndex <= maxCellIndex * numberOfPointsPerCell; ++cellIndex)
  {
    partPointIDs.insert(radiossCellConnectivity[cellIndex]);
  }
  std::vector<std::array<float, 3>> pointCoordinates(partPointIDs.size());
  points->Allocate(partPointIDs.size());
  radiossNodeIDs->Allocate(partPointIDs.size());
  radiossNodeIDs->SetName("NODE_ID");
  for (auto partPointID : partPointIDs)
  {
    auto vtkPointIndex = points->InsertNextPoint(&radiossPointCoordinates[3 * partPointID]);
    radiossPointIdToVTKPointIndex[partPointID] = vtkPointIndex;
    radiossNodeIDs->InsertNextValue(partPointID);
  }
}

void vtkRadiossAnimReader::ExtractPartPointData(const RadiossAnimDataModel::Quads& radiossQuads,
  const RadiossAnimDataModel::Nodes& radiossNodes, vtkIntArray* radiossNodeIDs,
  std::vector<vtkSmartPointer<vtkAbstractArray>>& pointDataArrays)
{
  // TODO.
}

VTK_ABI_NAMESPACE_END
