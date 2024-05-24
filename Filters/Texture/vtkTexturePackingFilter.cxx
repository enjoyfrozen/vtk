// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkTexturePackingFilter.h"

#include "vtkAppendPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkeigen/eigen/Geometry"

#include <vector>

VTK_ABI_NAMESPACE_BEGIN

vtkStandardNewMacro(vtkTexturePackingFilter);

namespace
{
//------------------------------------------------------------------------------
void GetFarthestPoints(const vtkSmartPointer<vtkPolyData>& curPD1,
  std::pair<vtkIdType, vtkIdType>& pointIds, Eigen::Vector3d& distanceVector)
{
  distanceVector.setZero();
  vtkIdType nPoint = curPD1->GetNumberOfPoints();
  for (vtkIdType i = 0; i < nPoint - 1; ++i)
  {
    for (vtkIdType j = i + 1; j < nPoint; ++j)
    {
      auto point0 = Eigen::Vector3d(curPD1->GetPoint(i));
      auto point1 = Eigen::Vector3d(curPD1->GetPoint(j));
      auto curDistanceVector = point1 - point0;
      if (curDistanceVector.norm() > distanceVector.norm())
      {
        distanceVector = curDistanceVector;
        pointIds.first = i;
        pointIds.second = j;
      }
    }
  }
}

//==============================================================================
struct TexelizedChart
{
  void Initialize(const vtkSmartPointer<vtkPolyData>& curPD1, double texelSize, int boundaryTexel)
  {
    this->CurPD1 = curPD1;
    this->TexelSize = texelSize;
    this->BoundaryTexel = boundaryTexel;
  }

  vtkSmartPointer<vtkPolyData> CurPD1;
  double TexelSize;
  Eigen::VectorXi TopHorizon;
  Eigen::VectorXi BottomHorizon;
  int LeftBound = 0;
  int RightBound = 0;
  int NumberOfTexel = 1;
  int BoundaryTexel = 1;

  int Compute();
};

//------------------------------------------------------------------------------
int TexelizedChart::Compute()
{
  vtkIdType nCell = this->CurPD1->GetNumberOfCells();
  std::vector<Eigen::VectorXd> cellHorizons;
  std::vector<int> cellLeftBounds;
  std::vector<int> cellRightBounds;

  cellHorizons.resize(nCell);
  cellLeftBounds.resize(nCell);
  cellRightBounds.resize(nCell);

  // compute left and right bounds
  for (vtkIdType i = 0; i < nCell; ++i)
  {
    auto* curCell = this->CurPD1->GetCell(i);

    /*
    // current vtkPolyDataTransformFilter changes polyline to line
    if (curCell->GetCellType() != VTK_POLY_LINE)
    {
      return 0;
    }
    */

    // TODO: extend to polyline with more than two points.

    double point0[3];
    double point1[3];
    curCell->GetPoints()->GetPoint(0, point0);
    curCell->GetPoints()->GetPoint(1, point1);
    double* pointLeft;
    double* pointRight;

    if (point0[0] < point1[0])
    {
      pointLeft = point0;
      pointRight = point1;
    }
    else
    {
      pointLeft = point1;
      pointRight = point0;
    }

    cellLeftBounds[i] = floor(pointLeft[0] / this->TexelSize);
    cellRightBounds[i] = ceil(pointRight[0] / this->TexelSize);

    this->LeftBound = (cellLeftBounds[i] < this->LeftBound ? cellLeftBounds[i] : this->LeftBound);
    this->RightBound =
      (cellRightBounds[i] > this->RightBound ? cellRightBounds[i] : this->RightBound);

    // compute cell horizon
    int nCellTexel = cellRightBounds[i] - cellLeftBounds[i] + 1;
    cellHorizons[i].resize(nCellTexel);
    for (int j = 0; j < nCellTexel; ++j)
    {
      // assume the left bound point is on the texel center
      cellHorizons[i][j] =
        ((pointRight[1] - pointLeft[1]) * j / (nCellTexel - 1.0) + pointLeft[1]) / this->TexelSize;
    }
  }

  // compute top and bottom horizons
  int preNumberOfTexel = this->RightBound - this->LeftBound + 1;
  Eigen::VectorXi preBottomHorizon(preNumberOfTexel);
  Eigen::VectorXi preTopHorizon(preNumberOfTexel);
  preBottomHorizon.setConstant(VTK_INT_MAX);
  preTopHorizon.setZero();

  for (vtkIdType i = 0; i < nCell; ++i)
  {
    int offset = cellLeftBounds[i] - this->LeftBound;
    int nCellTexel = static_cast<int>(cellHorizons[i].size());
    for (int j = 0; j < nCellTexel; ++j)
    {
      if (floor(cellHorizons[i][j]) < preBottomHorizon[j + offset])
      {
        preBottomHorizon[j + offset] = floor(cellHorizons[i][j]);
      }
      if (ceil(cellHorizons[i][j]) > preTopHorizon[j + offset])
      {
        preTopHorizon[j + offset] = ceil(cellHorizons[i][j]);
      }
    }
  }

  // add boundary texels for spacing purpose
  this->RightBound += this->BoundaryTexel;
  this->LeftBound -= this->BoundaryTexel;
  this->NumberOfTexel = this->RightBound - this->LeftBound + 1;

  Eigen::VectorXi corner(this->BoundaryTexel);
  for (int i = 0; i < this->BoundaryTexel; ++i)
  {
    corner[i] = i;
  }

  this->TopHorizon.resize(this->NumberOfTexel);
  this->TopHorizon.segment(this->BoundaryTexel, preNumberOfTexel) =
    preTopHorizon + (Eigen::VectorXi::Ones(preNumberOfTexel) * this->BoundaryTexel);
  this->TopHorizon.head(this->BoundaryTexel) = preTopHorizon.head(this->BoundaryTexel);
  this->TopHorizon.tail(this->BoundaryTexel) = preTopHorizon.tail(this->BoundaryTexel);

  this->BottomHorizon.resize(this->NumberOfTexel);
  this->BottomHorizon.segment(this->BoundaryTexel, preNumberOfTexel) =
    preBottomHorizon - (Eigen::VectorXi::Ones(preNumberOfTexel) * this->BoundaryTexel);
  this->BottomHorizon.head(this->BoundaryTexel) = preBottomHorizon.head(this->BoundaryTexel);
  this->BottomHorizon.tail(this->BoundaryTexel) = preBottomHorizon.tail(this->BoundaryTexel);

  return 1;
}
} // anonymous namespace

//------------------------------------------------------------------------------
vtkTexturePackingFilter::vtkTexturePackingFilter()
{
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(2);
}

//------------------------------------------------------------------------------
int vtkTexturePackingFilter::RequestData(vtkInformation* /*request*/,
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inPDSInfo0 = inputVector[ATLAS]->GetInformationObject(0);
  vtkInformation* inPDSInfo1 = inputVector[BOUNDARY]->GetInformationObject(0);
  vtkInformation* outPolyInfo0 = outputVector->GetInformationObject(UNIATLAS);
  vtkInformation* outPolyInfo1 = outputVector->GetInformationObject(HORIZON);

  std::cout << "vtkTexturePackingFilter INPUT TYPE "
            << vtkDataObject::GetData(inPDSInfo1)->GetClassName() << std::endl;

  // get the input and output
  auto* inputPDS0 = vtkPartitionedDataSet::GetData(inPDSInfo0);
  auto* inputPDS1 = vtkPartitionedDataSet::GetData(inPDSInfo1);
  auto* outputPD0 = vtkPolyData::GetData(outPolyInfo0);
  auto* outputPD1 = vtkPolyData::GetData(outPolyInfo1);

  // check inputs
  unsigned int nChart = inputPDS0->GetNumberOfPartitions();
  if (nChart != inputPDS1->GetNumberOfPartitions())
  {
    vtkErrorMacro("Numbers of partitions do not agree in inputPDSs.");
  }

  // ivars
  std::vector<vtkNew<vtkTransform>> transforms; // transform for each polydatas
  std::vector<std::pair<vtkIdType, vtkIdType>>
    farthestPoints; // farthest points for re-orienting polydatas
  std::vector<vtkNew<vtkPolyData>> transformedPDs0;
  std::vector<vtkNew<vtkPolyData>> transformedPDs1;
  std::vector<TexelizedChart> texelizedCharts;
  Eigen::VectorXi globalHorizon;

  // pre-allocation
  transforms.resize(nChart);
  farthestPoints.resize(nChart);
  transformedPDs0.resize(nChart);
  transformedPDs1.resize(nChart);
  texelizedCharts.resize(nChart);
  globalHorizon.resize(this->TextureMapWidth);
  globalHorizon.setZero();

  for (unsigned int i = 0; i < nChart; ++i)
  {
    transformedPDs0[i]->DeepCopy(vtkPolyData::SafeDownCast(inputPDS0->GetPartition(i)));
    transformedPDs1[i]->DeepCopy(vtkPolyData::SafeDownCast(inputPDS1->GetPartition(i)));
  }

  // re-position polydatas with the farthest points placed along y axis
  for (unsigned int i = 0; i < nChart; ++i)
  {
    vtkPolyData* curPD0 = transformedPDs0[i];
    vtkPolyData* curPD1 = transformedPDs1[i];

    // get point pair with the largest distance.
    std::pair<vtkIdType, vtkIdType> pointIds;
    Eigen::Vector3d distanceVector;
    GetFarthestPoints(curPD1, pointIds, distanceVector);

    // 2d rotation matrix extraction
    Eigen::Rotation2D<double> rot2(vtkMath::Pi() / 2);
    Eigen::Matrix2d preRot;
    Eigen::Matrix2d postRot;
    preRot.col(0) = Eigen::Vector2d(distanceVector.x(), distanceVector.y());
    preRot.col(1) =
      rot2.toRotationMatrix() * Eigen::Vector2d(distanceVector.x(), distanceVector.y());
    postRot.coeffRef(1, 0) = distanceVector.norm();
    postRot.coeffRef(0, 1) = -distanceVector.norm();
    Eigen::Matrix2d rotMat = postRot * preRot.inverse();

    // construct transform
    vtkNew<vtkTransform> transform;
    vtkNew<vtkMatrix4x4> transformMatrix;
    transformMatrix->SetElement(0, 0, rotMat.coeff(0, 0));
    transformMatrix->SetElement(0, 1, rotMat.coeff(0, 1));
    transformMatrix->SetElement(1, 0, rotMat.coeff(1, 0));
    transformMatrix->SetElement(1, 1, rotMat.coeff(1, 1));
    transform->SetMatrix(transformMatrix);
    double translation[3];
    curPD1->GetPoint(pointIds.first, translation);
    transform->Translate(-translation[0], -translation[1], -translation[2]);
    transform->Update();

    // apply the same transform to both atlas and boundary
    vtkNew<vtkTransformPolyDataFilter> transformFilter;
    transformFilter->SetTransform(transform);
    transformFilter->SetInputDataObject(curPD0);
    transformFilter->Update();
    curPD0->ShallowCopy(transformFilter->GetOutputDataObject(0));

    // TODO: why transformFilter change cellTypes from polyline to line
    transformFilter->SetInputDataObject(curPD1);
    transformFilter->Update();
    curPD1->ShallowCopy(transformFilter->GetOutputDataObject(0));
  }

  // texelization
  for (unsigned int i = 0; i < nChart; ++i)
  {
    texelizedCharts[i].Initialize(transformedPDs1[i], this->TexelSize, this->BoundaryTexel);
    if (!texelizedCharts[i].Compute())
    {
      vtkErrorMacro("Failed to compute texelized charts.");
      return 0;
    }
  }

  // Sort partitions with heights
  std::vector<std::pair<int, int>> partitionHeightPair;
  partitionHeightPair.resize(nChart);
  for (unsigned int i = 0; i < nChart; ++i)
  {
    partitionHeightPair[i] = std::make_pair(i, texelizedCharts[i].TopHorizon.maxCoeff());
  }

  std::sort(partitionHeightPair.begin(), partitionHeightPair.end(),
    [](const std::pair<int, int>& a, const std::pair<int, int>& b) { return a.second > b.second; });

  // packing
  for (unsigned int i = 0; i < nChart; ++i)
  {
    auto curChart = texelizedCharts[partitionHeightPair[i].first];
    int maxOffset = this->TextureMapWidth - curChart.NumberOfTexel;
    int minGapArea = VTK_INT_MAX;
    int minGapOffset = 0;

    // displacement for final chart position
    int minGapX = 0;
    int minGapY = 0;

    if (maxOffset < 0)
    {
      vtkErrorMacro("Chart width is larger than texture map width.");
      return 0;
    }

    // loop through all possible offsets
    for (int j = 0; j <= maxOffset; j += this->StepSize)
    {
      auto gap = curChart.BottomHorizon - globalHorizon.middleRows(j, curChart.NumberOfTexel);
      int curGapArea = gap.sum() - gap.minCoeff() * curChart.NumberOfTexel;
      if (curGapArea < minGapArea)
      {
        minGapArea = curGapArea;
        minGapOffset = j;
        minGapX = -curChart.LeftBound + j;
        minGapY = -gap.minCoeff();
      }
    }

    // update horizon
    globalHorizon.middleRows(minGapOffset, curChart.NumberOfTexel) =
      curChart.TopHorizon + (Eigen::VectorXi::Ones(curChart.NumberOfTexel) * minGapY);

    // translate
    vtkNew<vtkTransform> transform;
    transform->Translate((double)minGapX * this->TexelSize, (double)minGapY * this->TexelSize, 0);
    vtkNew<vtkTransformPolyDataFilter> transformFilter;
    transformFilter->SetTransform(transform);

    vtkPolyData* curPD0 = transformedPDs0[partitionHeightPair[i].first];
    transformFilter->SetInputDataObject(curPD0);
    transformFilter->Update();
    curPD0->ShallowCopy(transformFilter->GetOutputDataObject(0));
  }

  // append combine multiple meshes into one polydata
  vtkNew<vtkAppendPolyData> appendFilter;
  for (unsigned int i = 0; i < nChart; ++i)
  {
    appendFilter->AddInputDataObject(transformedPDs0[i]);
  }
  appendFilter->Update();
  outputPD0->ShallowCopy(appendFilter->GetOutputDataObject(0));

  // point for horizon
  vtkNew<vtkPoints> horizonPoints;
  horizonPoints->Allocate(this->TextureMapWidth);
  for (int i = 0; i < this->TextureMapWidth; ++i)
  {
    horizonPoints->InsertNextPoint(i * this->TexelSize, globalHorizon[i] * this->TexelSize, 0);
  }
  outputPD1->SetPoints(horizonPoints);
  outputPD1->Allocate(this->TextureMapWidth - 1);
  for (int i = 0; i < this->TextureMapWidth - 1; ++i)
  {
    vtkNew<vtkIdList> cell;
    cell->SetNumberOfIds(2);
    cell->SetId(0, i);
    cell->SetId(1, i + 1);
    outputPD1->InsertNextCell(VTK_POLY_LINE, cell);
  }

  return 1;
}

//------------------------------------------------------------------------------
int vtkTexturePackingFilter::FillInputPortInformation(int /* port */, vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPartitionedDataSet");
  return 1;
}

//------------------------------------------------------------------------------
void vtkTexturePackingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Texel size: " << this->TexelSize << "\n";
  os << indent << "Texture map width: " << this->TextureMapWidth << " texels\n";
  // os << indent << "Texture map height: " << m_textureMapHeight << "\n";
}
VTK_ABI_NAMESPACE_END
