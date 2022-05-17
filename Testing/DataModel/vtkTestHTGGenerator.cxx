/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTestHTGGenerator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyrgight notice for more information.

=========================================================================*/

#include "vtkTestHTGGenerator.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkHyperTree.h"
#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeGridNonOrientedCursor.h"

vtkStandardNewMacro(vtkTestHTGGenerator);

vtkTestHTGGenerator::vtkTestHTGGenerator()
  : CustomBalanced(0)
  , CustomDim(2)
  , CustomFactor(2)
  , CustomDepth(2)
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);

  this->AppropriateOutput = true;

  this->CustomExtent.resize(4);
  this->CustomExtent[0] = this->CustomExtent[2] = 0.0;
  this->CustomExtent[1] = this->CustomExtent[3] = 1.0;

  this->CustomSubdivisions.resize(2);
  std::fill(this->CustomSubdivisions.begin(), this->CustomSubdivisions.end(), 2);
}

int vtkTestHTGGenerator::FillOutputPortInformation(int port, vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkHyperTreeGrid");
  return 1;
}

int vtkTestHTGGenerator::RequestInformation(
  vtkInformation* req, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  if (!this->Superclass::RequestInformation(req, inputVector, outputVector))
  {
    return 0;
  }
  std::array<int, 6> wholeExtent = { 0, 1, 0, 1, 0, 1 };
  int dimension = 2;
  int depth = 3;
  switch (this->HTGMode)
  {
    case UNBALANCED_3DEPTH_2BRANCH_2X3:
      wholeExtent[3] = 2;
      break;
    case BALANCED_3DEPTH_2BRANCH_2X3:
      wholeExtent[3] = 2;
      break;
    case UNBALANCED_2DEPTH_3BRANCH_3X3:
      wholeExtent[1] = wholeExtent[3] = 2;
      depth = 2;
      break;
    case BALANCED_4DEPTH_3BRANCH_2X2:
      depth = 4;
      break;
    case UNBALANCED_3DEPTH_2BRANCH_3X2X3:
      wholeExtent[1] = wholeExtent[5] = 2;
      dimension = 3;
      break;
    case BALANCED_2DEPTH_3BRANCH_3X3X2:
      wholeExtent[1] = wholeExtent[3] = 2;
      dimension = 3;
      depth = 2;
      break;
    case CUSTOM:
      std::copy(this->CustomExtent.begin(), this->CustomExtent.end(), wholeExtent.begin());
      dimension = this->CustomDim;
      depth = this->CustomDepth;
    default:
      vtkErrorMacro("No suitable HTG mode found.");
      return 0;
  }

  vtkInformation* info = outputVector->GetInformationObject(0);
  info->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), wholeExtent.begin(), 6);
  info->Set(vtkHyperTreeGrid::LEVELS(), depth);
  info->Set(vtkHyperTreeGrid::DIMENSION(), dimension);
  info->Set(vtkAlgorithm::CAN_PRODUCE_SUB_EXTENT(), 0);
  return 1;
}

int vtkTestHTGGenerator::RequestData(
  vtkInformation*, vtkInformationVector**, vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  if (!outInfo)
  {
    vtkErrorMacro("Output information not found");
    return 0;
  }
  vtkHyperTreeGrid* HTG = vtkHyperTreeGrid::GetData(outInfo);
  if (!HTG)
  {
    vtkErrorMacro("Could not get HyperTreeGrid output");
    return 0;
  }

  if (!this->ProcessTrees(nullptr, HTG))
  {
    return 0;
  }
  return 1;
}

int vtkTestHTGGenerator::ProcessTrees(vtkHyperTreeGrid*, vtkDataObject* HTGObj)
{
  vtkHyperTreeGrid* HTG = vtkHyperTreeGrid::SafeDownCast(HTGObj);
  switch (this->HTGMode)
  {
    case UNBALANCED_3DEPTH_2BRANCH_2X3:
      generateUnbalanced3DepthQuadTree2x3(HTG);
      break;
    case BALANCED_3DEPTH_2BRANCH_2X3:
      generateBalanced3DepthQuadTree2x3(HTG);
      break;
    case UNBALANCED_2DEPTH_3BRANCH_3X3:
      generateUnbalanced2Depth3BranchTree3x3(HTG);
      break;
    case BALANCED_4DEPTH_3BRANCH_2X2:
      generateBalanced4Depth3BranchTree2x2(HTG);
      break;
    case UNBALANCED_3DEPTH_2BRANCH_3X2X3:
      generateUnbalanced3DepthOctTree3x2x3(HTG);
      break;
    case BALANCED_2DEPTH_3BRANCH_3X3X2:
      generateBalanced2Depth3BranchTree3x3x2(HTG);
      break;
    case CUSTOM:
      if (!generateCustom(HTG))
      {
        vtkErrorMacro("Could not generate custom HyperTreeGrid");
        return 0;
      }
      break;
    default:
      vtkErrorMacro("Unsupported HTG mode");
      return 0;
  }
  return 1;
}

void vtkTestHTGGenerator::generateUnbalanced(vtkHyperTreeGrid* HTG, int dim, int factor, int depth,
  const std::vector<double>& extent, const std::vector<int>& subdivisions)
{
  this->preprocess(HTG, dim, factor, extent, subdivisions);

  vtkNew<vtkDoubleArray> levels;
  levels->SetName("Depth");
  levels->SetNumberOfComponents(1);
  levels->SetNumberOfTuples(0);
  HTG->GetCellData()->AddArray(levels);

  auto cursor = vtk::TakeSmartPointer(HTG->NewNonOrientedCursor(0, true));
  cursor->GetTree()->SetGlobalIndexStart(0);
  levels->InsertValue(0, 0);
  for (vtkIdType l = 0; l < depth; l++)
  {
    cursor->SubdivideLeaf();
    int numChildren = cursor->GetNumberOfChildren();
    for (int iChild = 0; iChild < numChildren; iChild++)
    {
      cursor->ToChild(iChild);
      vtkIdType vertexId = cursor->GetVertexId();
      vtkIdType globId = cursor->GetTree()->GetGlobalIndexFromLocal(vertexId);
      levels->InsertValue(globId, l + 1);
      cursor->ToParent();
    }
    cursor->ToChild(0);
  }
  vtkIdType treeOffset = cursor->GetTree()->GetNumberOfVertices();

  int nTrees = 1;
  for (auto sub : subdivisions)
  {
    nTrees *= (sub - 1);
  }
  for (int iT = 1; iT < nTrees; iT++)
  {
    cursor = vtk::TakeSmartPointer(HTG->NewNonOrientedCursor(iT, true));
    vtkHyperTree* tree = cursor->GetTree();
    tree->SetGlobalIndexStart(treeOffset);
    vtkIdType globId = tree->GetGlobalIndexFromLocal(0);
    levels->InsertValue(globId, 0);
    treeOffset += tree->GetNumberOfVertices();
  }
}

void vtkTestHTGGenerator::generateBalanced(vtkHyperTreeGrid* HTG, int dim, int factor, int depth,
  const std::vector<double>& extent, const std::vector<int>& subdivisions)
{
  this->preprocess(HTG, dim, factor, extent, subdivisions);

  vtkNew<vtkDoubleArray> levels;
  levels->SetName("Depth");
  levels->SetNumberOfComponents(1);
  levels->SetNumberOfTuples(0);
  HTG->GetCellData()->AddArray(levels);

  vtkIdType treeOffset = 0;
  int nTrees = 1;
  for (auto sub : subdivisions)
  {
    nTrees *= (sub - 1);
  }
  for (int iT = 0; iT < nTrees; iT++)
  {
    auto cursor = vtk::TakeSmartPointer(HTG->NewNonOrientedCursor(iT, true));
    cursor->GetTree()->SetGlobalIndexStart(treeOffset);
    this->recurseBalanced(cursor, levels, depth);
    treeOffset += cursor->GetTree()->GetNumberOfVertices();
  }
}

void vtkTestHTGGenerator::recurseBalanced(
  vtkHyperTreeGridNonOrientedCursor* cursor, vtkDoubleArray* levels, const int maxDepth)
{
  vtkIdType vertexId = cursor->GetVertexId();
  vtkHyperTree* tree = cursor->GetTree();
  vtkIdType globId = tree->GetGlobalIndexFromLocal(vertexId);
  vtkIdType thisLevel = cursor->GetLevel();

  levels->InsertValue(globId, thisLevel);

  if (cursor->IsLeaf())
  {
    if (thisLevel < maxDepth)
    {
      cursor->SubdivideLeaf();
      this->recurseBalanced(cursor, levels, maxDepth);
    }
  }
  else
  {
    int numChildren = cursor->GetNumberOfChildren();
    for (int iChild = 0; iChild < numChildren; iChild++)
    {
      cursor->ToChild(iChild);
      this->recurseBalanced(cursor, levels, maxDepth);
      cursor->ToParent();
    }
  }
}

void vtkTestHTGGenerator::preprocess(vtkHyperTreeGrid* HTG, int dim, int factor,
  const std::vector<double>& extent, const std::vector<int>& subdivisions)
{
  if (extent.size() < static_cast<unsigned int>(2 * dim))
  {
    vtkErrorMacro("Supplied extent is not long enough");
    return;
  }
  if (subdivisions.size() < static_cast<unsigned int>(dim))
  {
    vtkErrorMacro("Supplied subdivisions is not long enough");
    return;
  }
  HTG->Initialize();
  std::array<int, 3> subdivisions3d = { 1, 1, 1 };
  std::copy(subdivisions.begin(), subdivisions.end(), subdivisions3d.begin());
  HTG->SetDimensions(subdivisions3d.begin());
  HTG->SetBranchFactor(factor);
  for (int d = 0; d < dim; d++)
  {
    vtkNew<vtkDoubleArray> vtkCoords;
    vtkCoords->SetNumberOfComponents(1);
    vtkCoords->SetNumberOfTuples(subdivisions[d]);
    double step = (extent[d * 2 + 1] - extent[d * 2]) / (subdivisions[d] - 1.0);
    for (int i = 0; i < subdivisions[d]; i++)
    {
      vtkCoords->InsertValue(i, extent[d * 2] + step * i);
    }
    switch (d)
    {
      case (0):
        HTG->SetXCoordinates(vtkCoords);
        break;
      case (1):
        HTG->SetYCoordinates(vtkCoords);
        break;
      case (2):
        HTG->SetZCoordinates(vtkCoords);
        break;
    }
  }
}

void vtkTestHTGGenerator::generateUnbalanced3DepthQuadTree2x3(vtkHyperTreeGrid* HTG)
{
  std::vector<double> extent = { -1.0, 1.0, -1.0, 1.0 };
  std::vector<int> subdivisions = { 2, 3 };
  generateUnbalanced(HTG, 2, 2, 3, extent, subdivisions);
}

void vtkTestHTGGenerator::generateBalanced3DepthQuadTree2x3(vtkHyperTreeGrid* HTG)
{
  std::vector<double> extent = { -1.0, 1.0, -1.0, 1.0 };
  std::vector<int> subdivisions = { 2, 3 };
  generateBalanced(HTG, 2, 2, 3, extent, subdivisions);
}

void vtkTestHTGGenerator::generateUnbalanced2Depth3BranchTree3x3(vtkHyperTreeGrid* HTG)
{
  std::vector<double> extent = { -1.0, 1.0, -1.0, 1.0 };
  std::vector<int> subdivisions = { 3, 3 };
  generateUnbalanced(HTG, 2, 3, 2, extent, subdivisions);
}

void vtkTestHTGGenerator::generateBalanced4Depth3BranchTree2x2(vtkHyperTreeGrid* HTG)
{
  std::vector<double> extent = { -1.0, 1.0, -1.0, 1.0 };
  std::vector<int> subdivisions = { 2, 2 };
  generateBalanced(HTG, 2, 3, 4, extent, subdivisions);
}

void vtkTestHTGGenerator::generateUnbalanced3DepthOctTree3x2x3(vtkHyperTreeGrid* HTG)
{
  std::vector<double> extent = { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 };
  std::vector<int> subdivisions = { 3, 2, 3 };
  generateUnbalanced(HTG, 3, 2, 3, extent, subdivisions);
}

void vtkTestHTGGenerator::generateBalanced2Depth3BranchTree3x3x2(vtkHyperTreeGrid* HTG)
{
  std::vector<double> extent = { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 };
  std::vector<int> subdivisions = { 3, 3, 2 };
  generateBalanced(HTG, 3, 3, 2, extent, subdivisions);
}

int vtkTestHTGGenerator::generateCustom(vtkHyperTreeGrid* HTG)
{
  if (this->CustomExtent.size() < static_cast<unsigned int>(this->CustomDim * 2))
  {
    vtkErrorMacro("Custom extent array is not long enough");
    return 0;
  }

  if (this->CustomSubdivisions.size() < static_cast<unsigned int>(this->CustomDim))
  {
    vtkErrorMacro("Custom subdivisions array is not long enough");
    return 0;
  }

  if (this->CustomBalanced)
  {
    generateBalanced(HTG, this->CustomDim, this->CustomFactor, this->CustomDepth,
      this->CustomExtent, this->CustomSubdivisions);
  }
  else
  {
    generateUnbalanced(HTG, this->CustomDim, this->CustomFactor, this->CustomDepth,
      this->CustomExtent, this->CustomSubdivisions);
  }
  return 1;
}

void vtkTestHTGGenerator::SetCustomExtent(int extentSize, double* extent)
{
  this->CustomExtent.resize(extentSize);
  std::copy(extent, extent + extentSize, this->CustomExtent.begin());
}

void vtkTestHTGGenerator::SetCustomSubdivisions(int subSize, int* subdivisions)
{
  this->CustomSubdivisions.resize(subSize);
  std::copy(subdivisions, subdivisions + subSize, this->CustomSubdivisions.begin());
}
