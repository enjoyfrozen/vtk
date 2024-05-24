// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkLSCMFilter.h"

#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkSMPTools.h"

#include "vtkeigen/eigen/Dense"

#include <map>
#include <set>
#include <vector>

using namespace std;
using namespace Eigen;

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkLSCMFilter);

namespace
{
//------------------------------------------------------------------------------
void ComputeCotMatrix(const vtkSmartPointer<vtkPolyData>& inputPD, Eigen::MatrixXd& lapMat)
{
  using namespace Eigen;

  vtkIdType nNode = inputPD->GetNumberOfPoints();
  lapMat.resize(nNode, nNode);
  lapMat.setZero();
  auto* points = inputPD->GetPoints();
  vtkNew<vtkIdList> tri;
  tri->SetNumberOfIds(3);

  for (int i = 0; i < inputPD->GetNumberOfCells(); i++)
  {
    inputPD->GetCellPoints(i, tri);

    Vector3d p0(points->GetPoint(tri->GetId(0)));
    Vector3d p1(points->GetPoint(tri->GetId(1)));
    Vector3d p2(points->GetPoint(tri->GetId(2)));

    double area = 0.5 * ((p1 - p0).cross(p2 - p0)).norm();
    double a = (p2 - p1).norm();
    double b = (p0 - p2).norm();
    double c = (p1 - p0).norm();
    double cotList[3] = { (b * b + c * c - a * a) / 4 / area, (a * a + c * c - b * b) / 4 / area,
      (b * b + a * a - c * c) / 4 / area };

    int idx[5] = { 0, 1, 2, 0, 1 };
    for (int j = 0; j < 3; j++)
    {
      lapMat.coeffRef(tri->GetId(idx[j]), tri->GetId(idx[j])) -=
        0.5 * (cotList[idx[j + 1]] + cotList[idx[j + 2]]);
      lapMat.coeffRef(tri->GetId(idx[j]), tri->GetId(idx[j + 1])) += 0.5 * cotList[idx[j + 2]];
      lapMat.coeffRef(tri->GetId(idx[j]), tri->GetId(idx[j + 2])) += 0.5 * cotList[idx[j + 1]];
    }
  }
}

//------------------------------------------------------------------------------
void ComputeAreaMatrix(const vtkSmartPointer<vtkPolyData>& inputPD, Eigen::MatrixXd& aMat2)
{
  using namespace Eigen;

  int nNode = inputPD->GetNumberOfPoints();
  aMat2.resize(2 * nNode, 2 * nNode);
  aMat2.setZero();

  vtkNew<vtkIdList> tri;
  tri->SetNumberOfIds(3);
  for (int i = 0; i < inputPD->GetNumberOfCells(); i++)
  {
    double a = 0.25;
    inputPD->GetCellPoints(i, tri);

    int idx00 = tri->GetId(0);
    int idx10 = tri->GetId(1);
    int idx20 = tri->GetId(2);

    int idx01 = idx00 + nNode;
    int idx11 = idx10 + nNode;
    int idx21 = idx20 + nNode;

    aMat2.coeffRef(idx11, idx00) += a;
    aMat2.coeffRef(idx10, idx01) -= a;
    aMat2.coeffRef(idx01, idx10) -= a;
    aMat2.coeffRef(idx00, idx11) += a;

    aMat2.coeffRef(idx21, idx10) += a;
    aMat2.coeffRef(idx20, idx11) -= a;
    aMat2.coeffRef(idx11, idx20) -= a;
    aMat2.coeffRef(idx10, idx21) += a;

    aMat2.coeffRef(idx01, idx20) += a;
    aMat2.coeffRef(idx00, idx21) -= a;
    aMat2.coeffRef(idx21, idx00) -= a;
    aMat2.coeffRef(idx20, idx01) += a;
  }
}

//------------------------------------------------------------------------------
void ComputeLSCM(VectorXd& lVec2, MatrixXd& lapMat2, MatrixXd& aMat2,
  const vtkSmartPointer<vtkIdList>& anchorIds, VectorXd& anchorDisps)
{

  auto nNode = lapMat2.rows() / 2;
  int nAnchor = anchorIds->GetNumberOfIds();

  // lhs matrix
  MatrixXd lMat2 = MatrixXd::Zero(2 * (nNode + nAnchor), 2 * (nNode + nAnchor));
  lMat2.topLeftCorner(2 * nNode, 2 * nNode) =
    (-lapMat2 + 2 * aMat2).transpose() * (-lapMat2 + 2 * aMat2);
  for (int i = 0; i < nAnchor; ++i)
  {
    lMat2.coeffRef(2 * (nNode + i), anchorIds->GetId(i)) = 1;
    lMat2.coeffRef(2 * (nNode + i) + 1, anchorIds->GetId(i) + nNode) = 1;
  }
  lMat2.topRightCorner(2 * nNode, 2 * nAnchor) =
    lMat2.bottomLeftCorner(2 * nAnchor, 2 * nNode).transpose();

  // show rank using column pivoting
  ColPivHouseholderQR<MatrixXd> qr_decomp(lMat2);
  // vtkGenericWarningMacro("lMat2 rank = "<< qr_decomp.rank());

  // rhs vector
  VectorXd rVec2 = VectorXd::Zero(2 * (nNode + nAnchor));
  for (int i = 0; i < nAnchor; ++i)
  {
    rVec2.coeffRef(2 * (nNode + i)) = anchorDisps(2 * i);
    rVec2.coeffRef(2 * (nNode + i) + 1) = anchorDisps(2 * i + 1);
  }

  // solve
  lVec2 = lMat2.colPivHouseholderQr().solve(rVec2);
}

//==============================================================================
class ComputeLocalCoordinates
{
public:
  explicit ComputeLocalCoordinates(vtkSmartPointer<vtkPartitionedDataSet> outputPDS0,
    vtkSmartPointer<vtkPartitionedDataSet> outputPDS1, std::vector<double> chartAreas)
    : OutputPDS0(std::move(outputPDS0))
    , OutputPDS1(std::move(outputPDS1))
    , ChartAreas(std::move(chartAreas))
  {
  }
  vtkSmartPointer<vtkPartitionedDataSet> OutputPDS0;
  vtkSmartPointer<vtkPartitionedDataSet> OutputPDS1;
  std::vector<double> ChartAreas;
  void operator()(vtkIdType begin, vtkIdType end);
};

//------------------------------------------------------------------------------
void ComputeLocalCoordinates::operator()(vtkIdType begin, vtkIdType end)
{
  for (vtkIdType i = begin; i < end; ++i)
  {
    auto* curPD0 = vtkPolyData::SafeDownCast(this->OutputPDS0->GetPartition(i));
    auto* curPD1 = vtkPolyData::SafeDownCast(this->OutputPDS1->GetPartition(i));
    MatrixXd lapMat;
    MatrixXd lapMat2;
    MatrixXd aMat2;
    vtkIdType nNode = curPD0->GetNumberOfPoints();

    // Compute Laplacian matrix and concatenate it into 2D
    ComputeCotMatrix(curPD0, lapMat);
    lapMat2.resize(2 * nNode, 2 * nNode);
    lapMat2.setZero();
    lapMat2.topLeftCorner(nNode, nNode) = lapMat;
    lapMat2.bottomRightCorner(nNode, nNode) = lapMat;

    // Compute Area matrix
    ComputeAreaMatrix(curPD0, aMat2);

    // Search for two points farthest from each other as the anchor points and
    // use their original distance as the anchored distance
    int anchorId0 = 0;
    int anchorId1 = 0;
    double anchorDist = 0;
    for (int j = 0; j < nNode - 1; j++)
    {
      for (int k = j + 1; k < nNode; k++)
      {
        Vector3d p0(curPD0->GetPoint(j));
        Vector3d p1(curPD0->GetPoint(k));
        double pDist = (p0 - p1).norm();
        if (pDist > anchorDist)
        {
          anchorDist = pDist;
          anchorId0 = j;
          anchorId1 = k;
        }
      }
    }

    vtkNew<vtkIdList> anchorIds;
    anchorIds->SetNumberOfIds(2);
    anchorIds->SetId(0, anchorId0);
    anchorIds->SetId(1, anchorId1);

    VectorXd anchorDisps = VectorXd::Zero(4);
    anchorDisps(3) = anchorDist;

    // least square fitting with linear constraints using KKT equations
    // lhs matrix
    VectorXd lVec2(2 * (nNode + 4));
    lVec2.setZero();
    ComputeLSCM(lVec2, lapMat2, aMat2, anchorIds, anchorDisps);

    // temporarily set point locations to planeXY (UV) for computing area
    for (vtkIdType j = 0; j < nNode; j++)
    {
      curPD1->GetPoints()->SetPoint(j, lVec2(j), lVec2(j + nNode), 0);
    }

    // rescale mapped chart to its original area
    Vector3d curAreaVector = Vector3d::Zero();
    double rescaleFactor;
    for (int j = 0; j < curPD1->GetNumberOfCells(); j++)
    {
      vtkNew<vtkIdList> pointIds;
      curPD1->GetCellPoints(j, pointIds);
      auto p0 = Vector3d(curPD1->GetPoint(pointIds->GetId(0)));
      auto p1 = Vector3d(curPD1->GetPoint(pointIds->GetId(1)));
      curAreaVector += p0.cross(p1) / 2;
    }
    rescaleFactor = sqrt(this->ChartAreas[i] / curAreaVector.norm());
    for (vtkIdType j = 0; j < nNode; j++)
    {
      double x = rescaleFactor * lVec2[j];
      double y = rescaleFactor * lVec2[j + nNode];
      curPD0->GetPoints()->SetPoint(j, x, y, 0);
      curPD1->GetPoints()->SetPoint(j, x, y, 0);
    }
  }
}

} // anonymous namespace

//------------------------------------------------------------------------------
vtkLSCMFilter::vtkLSCMFilter()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(3);
}

//------------------------------------------------------------------------------
vtkPartitionedDataSet* vtkLSCMFilter::GetOutput(int port)
{
  return vtkPartitionedDataSet::SafeDownCast(this->GetOutputDataObject(port));
}

//------------------------------------------------------------------------------
int vtkLSCMFilter::RequestData(vtkInformation* /* request */, vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inPolyInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outPDSInfo0 = outputVector->GetInformationObject(ATLAS);
  vtkInformation* outPDSInfo1 = outputVector->GetInformationObject(BOUNDARY);
  vtkInformation* outPDSInfo2 = outputVector->GetInformationObject(SEGMENTS);

  // get the input and output
  auto* inputPD = vtkPolyData::GetData(inPolyInfo);
  auto* outputPDS0 = vtkPartitionedDataSet::GetData(outPDSInfo0);
  auto* outputPDS1 = vtkPartitionedDataSet::GetData(outPDSInfo1);
  auto* outputPDS2 = vtkPartitionedDataSet::GetData(outPDSInfo2);

  int association;
  auto* chartIds = this->GetInputArrayToProcess(CHART_ID, inputVector, association);
  if (!chartIds)
  {
    vtkErrorMacro("Chart ID cell-scalars must be provided.");
    return 0;
  }
  if (chartIds->GetNumberOfComponents() != 1)
  {
    vtkErrorMacro("Chart ID values must be scalar-valued; "
      << "the " << chartIds->GetName() << " array has " << chartIds->GetNumberOfComponents()
      << " != 1.");
    return 0;
  }
  if (association != vtkDataObject::CELL)
  {
    vtkErrorMacro("Chart ID scalars must be cell-data (but association was " << association << ".");
    return 0;
  }
  vtkDebugMacro("number of chart ids: " << chartIds->GetSize());

  // Check input
  vtkIdType nCell = inputPD->GetNumberOfCells();
  if (nCell < 1 || nCell != chartIds->GetNumberOfValues())
  {
    vtkErrorMacro(<< "Error in getting triangular surface/chart ids.!");
    return 0;
  }

  // Ivars
  // global chart id to output partition id
  map<vtkIdType, vtkIdType> chartMap;
  // input point id to in-chart point id map and its inverse map
  vector<map<vtkIdType, vtkIdType>> pointMaps;
  vector<map<vtkIdType, vtkIdType>> invPointMaps;
  // input cell id to in-chart cell id
  vector<vtkNew<vtkIdList>> cellMaps;
  // in-chart non-duplicated edge sets using global id
  vector<set<pair<vtkIdType, vtkIdType>>> edgeSets;
  // partition id and edge id on the opposite side of a boundary edge
  vector<vtkNew<vtkIdTypeArray>> oppPartIds;
  vector<vtkNew<vtkIdTypeArray>> oppEdgeIds;
  // original area of each chart
  vector<double> chartAreas;

  // fill ChartMap
  for (vtkIdType i = 0; i < nCell; i++)
  {
    if (inputPD->GetCellType(i) != VTK_TRIANGLE)
    {
      vtkErrorMacro(<< "Non-triangular element detected!");
      return 0;
    }

    vtkIdType curChartId = chartIds->GetTuple1(i);
    if (chartMap.find(curChartId) == chartMap.end())
    {
      chartMap[curChartId] = chartMap.size();
    }
  }

  // pre-allocation for outputPDS
  auto numberOfCharts = static_cast<vtkIdType>(chartMap.size());
  pointMaps.resize(numberOfCharts);
  invPointMaps.resize(numberOfCharts);
  cellMaps.resize(numberOfCharts);
  edgeSets.resize(numberOfCharts);
  oppEdgeIds.resize(numberOfCharts);
  oppPartIds.resize(numberOfCharts);
  outputPDS0->SetNumberOfPartitions(numberOfCharts);
  outputPDS1->SetNumberOfPartitions(numberOfCharts);
  outputPDS2->SetNumberOfPartitions(numberOfCharts);
  chartAreas.resize(numberOfCharts);

  for (vtkIdType i = 0; i < numberOfCharts; i++)
  {
    oppPartIds[i]->SetName("opposite partition id");

    oppEdgeIds[i]->SetName("opposite edge id");

    outputPDS0->SetPartition(i, vtkNew<vtkPolyData>());
    outputPDS1->SetPartition(i, vtkNew<vtkPolyData>());
    outputPDS2->SetPartition(i, vtkNew<vtkPolyData>());

    vtkNew<vtkPoints> points0;
    vtkNew<vtkPoints> points1;
    points0->Allocate(inputPD->GetNumberOfPoints());
    points1->Allocate(inputPD->GetNumberOfPoints());

    cellMaps[i]->Allocate(inputPD->GetNumberOfCells());

    auto* curPD0 = vtkPolyData::SafeDownCast(outputPDS0->GetPartition(i));
    auto* curPD1 = vtkPolyData::SafeDownCast(outputPDS1->GetPartition(i));
    curPD0->SetPoints(points0);
    curPD1->SetPoints(points1);
    curPD0->Allocate(inputPD->GetNumberOfCells());
    curPD1->Allocate(
      2 * inputPD->GetNumberOfCells()); // lower bound: 1.5 edges per triangle in closed surface
  }

  // store charts in outputPDS
  for (vtkIdType i = 0; i < nCell; i++)
  {
    auto curPartId = chartMap[chartIds->GetTuple1(i)]; // partition id
    auto* curPD0 = vtkPolyData::SafeDownCast(outputPDS0->GetPartition(curPartId));
    auto* curPD1 = vtkPolyData::SafeDownCast(outputPDS1->GetPartition(curPartId));
    auto& curPtMap = pointMaps[curPartId];
    auto& curInvMap = invPointMaps[curPartId];
    auto& curEdgeSet = edgeSets[curPartId];
    auto& curArea = chartAreas[curPartId];
    auto& curCellMap = cellMaps[curPartId];

    vtkNew<vtkIdList> triIdListGlobal;
    vtkNew<vtkIdList> triIdListLocal;
    triIdListGlobal->SetNumberOfIds(3);
    triIdListLocal->SetNumberOfIds(3);
    inputPD->GetCellPoints(i, triIdListGlobal);

    // insert points to outputPDS
    for (vtkIdType j = 0; j < 3; j++)
    {
      vtkIdType pointId = triIdListGlobal->GetId(j);
      if (curPtMap.find(pointId) == curPtMap.end())
      {
        auto* points0 = curPD0->GetPoints();
        auto* points1 = curPD1->GetPoints();
        points0->InsertNextPoint(inputPD->GetPoint(pointId));
        points1->InsertNextPoint(inputPD->GetPoint(pointId));
        curPtMap[pointId] = curPtMap.size();
        curInvMap[curPtMap.size() - 1] = pointId;
      }
      triIdListLocal->SetId(j, curPtMap[pointId]);
    }

    // insert triangles to outputPDS0 and update area
    curPD0->InsertNextCell(VTK_TRIANGLE, triIdListLocal);
    curCellMap->InsertNextId(i);
    auto p0 = Vector3d(inputPD->GetPoint(triIdListGlobal->GetId(0)));
    auto p1 = Vector3d(inputPD->GetPoint(triIdListGlobal->GetId(1)));
    auto p2 = Vector3d(inputPD->GetPoint(triIdListGlobal->GetId(2)));
    curArea += ((p1 - p0).cross(p2 - p0)).norm() / 2;

    // extract boundary edges using halfedges
    pair<vtkIdType, vtkIdType> polylines[3];
    pair<vtkIdType, vtkIdType> invPolylines[3];

    polylines[0] = make_pair(triIdListGlobal->GetId(1), triIdListGlobal->GetId(0));
    polylines[1] = make_pair(triIdListGlobal->GetId(2), triIdListGlobal->GetId(1));
    polylines[2] = make_pair(triIdListGlobal->GetId(0), triIdListGlobal->GetId(2));
    invPolylines[0] = make_pair(triIdListGlobal->GetId(0), triIdListGlobal->GetId(1));
    invPolylines[1] = make_pair(triIdListGlobal->GetId(1), triIdListGlobal->GetId(2));
    invPolylines[2] = make_pair(triIdListGlobal->GetId(2), triIdListGlobal->GetId(0));

    for (int j = 0; j < 3; ++j)
    {
      if (curEdgeSet.find(polylines[j]) == curEdgeSet.end())
      {
        curEdgeSet.insert(invPolylines[j]);
      }
      else
      {
        curEdgeSet.erase(polylines[j]);
      }
    }
  }

  vtkDebugMacro("chart area: " << chartAreas[0] << " " << chartAreas[1]);

  // store boundary edges in outPDS1
  for (vtkIdType i = 0; i < numberOfCharts; ++i)
  {
    auto* curPD1 = vtkPolyData::SafeDownCast(outputPDS1->GetPartition(i));
    auto& curEdgeSet = edgeSets[i];

    for (const auto& iter : curEdgeSet)
    {
      vtkNew<vtkIdList> edgeIdList;
      edgeIdList->SetNumberOfIds(2);
      edgeIdList->SetId(0, pointMaps[i][iter.first]);
      edgeIdList->SetId(1, pointMaps[i][iter.second]);
      curPD1->InsertNextCell(VTK_POLY_LINE, edgeIdList);
    }
  }

  // attach global vertex id to the chart points
  for (vtkIdType i = 0; i < numberOfCharts; i++)
  {
    auto* curPD = vtkPolyData::SafeDownCast(outputPDS0->GetPartition(i));
    auto* curPointData = curPD->GetPointData();

    vtkNew<vtkIdList> inputPDList;
    vtkNew<vtkIdList> curPDList;
    inputPDList->Allocate(curPD->GetNumberOfPoints());
    curPDList->Allocate(curPD->GetNumberOfPoints());

    for (auto& iter : pointMaps[i])
    {
      inputPDList->InsertNextId(iter.first);
      curPDList->InsertNextId(iter.second);
    }

    curPointData->CopyAllOn();
    curPointData->CopyAllocate(inputPD->GetPointData(), curPD->GetNumberOfPoints());
    curPointData->CopyData(inputPD->GetPointData(), inputPDList, curPDList);
  }

  // attach global cell id and chart id to the chart cells
  for (vtkIdType i = 0; i < numberOfCharts; i++)
  {
    auto* curPD = vtkPolyData::SafeDownCast(outputPDS0->GetPartition(i));
    auto* curCD = curPD->GetCellData();

    vtkNew<vtkIdList> curCDList;
    curCDList->Allocate(curPD->GetNumberOfCells());
    for (int j = 0; j < curPD->GetNumberOfCells(); ++j)
    {
      curCDList->InsertNextId(j);
    }

    curCD->CopyAllOn();
    curCD->CopyAllocate(inputPD->GetCellData(), curPD->GetNumberOfCells());
    curCD->CopyData(inputPD->GetCellData(), cellMaps[i], curCDList);
  }

  // attach the opposite side cell and partition ids to the boundary polylines
  for (vtkIdType i = 0; i < numberOfCharts; ++i)
  {
    auto* curPD = vtkPolyData::SafeDownCast(outputPDS1->GetPartition(i));

    // set the opposite side partition and edge id to -1 by default
    oppEdgeIds[i]->SetNumberOfValues(curPD->GetNumberOfCells());
    oppPartIds[i]->SetNumberOfValues(curPD->GetNumberOfCells());
    for (int j = 0; j < curPD->GetNumberOfCells(); ++j)
    {
      oppEdgeIds[i]->SetValue(j, -1);
      oppPartIds[i]->SetValue(j, -1);
    }

    // search and assign opposite edge
    for (vtkIdType j = 0; j < numberOfCharts; ++j)
    {
      for (vtkIdType ii = 0; ii < curPD->GetNumberOfCells(); ++ii)
      {
        vtkNew<vtkIdList> curCellPts;
        curPD->GetCellPoints(ii, curCellPts);

        auto curEdgePair =
          make_pair(invPointMaps[i][curCellPts->GetId(1)], invPointMaps[i][curCellPts->GetId(0)]);
        if (edgeSets[j].find(curEdgePair) != edgeSets[j].end())
        {
          oppPartIds[i]->SetValue(ii, j);

          auto* curPD1 = vtkPolyData::SafeDownCast(outputPDS1->GetPartition(j));
          vtkNew<vtkIdList> curPtCells1;
          vtkIdType ptId = pointMaps[j][curEdgePair.first];

          curPD1->GetPointCells(ptId, curPtCells1);

          for (vtkIdType k = 0; k < curPtCells1->GetNumberOfIds(); ++k)
          {
            vtkIdType cellIdJ = curPtCells1->GetId(k);
            if (invPointMaps[j][curPD1->GetCell(cellIdJ)->GetPointId(0)] ==
              invPointMaps[i][curCellPts->GetId(1)])
            {
              oppEdgeIds[i]->SetValue(ii, curPtCells1->GetId(k));
            }
          }
        }
      }
    }

    // attach attributes to the outputPDS1
    curPD->GetCellData()->SetActiveScalars("opposite partition id");
    curPD->GetCellData()->SetScalars(oppPartIds[i]);
    curPD->GetCellData()->SetActiveScalars("opposite edge id");
    curPD->GetCellData()->SetScalars(oppEdgeIds[i]);
  }

  // Copy segments
  for (vtkIdType i = 0; i < numberOfCharts; ++i)
  {
    auto* curPD0 = vtkPolyData::SafeDownCast(outputPDS0->GetPartition(i));
    auto* curPD2 = vtkPolyData::SafeDownCast(outputPDS2->GetPartition(i));
    curPD2->DeepCopy(curPD0);
  }

  // Compute least-square conformal mapping for each chart
  ComputeLocalCoordinates func(outputPDS0, outputPDS1, chartAreas);
  vtkSMPTools::For(0, numberOfCharts, func);

  return 1;
}

//------------------------------------------------------------------------------
int vtkLSCMFilter::FillOutputPortInformation(int /*port*/, vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPartitionedDataSet");
  return 1;
}

//------------------------------------------------------------------------------
void vtkLSCMFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Fixed Nodes in Charts: "
     << "\n";
}
VTK_ABI_NAMESPACE_END
