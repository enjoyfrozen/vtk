// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkTextureAtlasGenerator.h"

#include "vtkAppendDataSets.h"
#include "vtkDistanceToFeature.h"
#include "vtkFloatArray.h"
#include "vtkGenerateGlobalIds.h"
#include "vtkGrowCharts.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLSCMFilter.h"
#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkNewFeatureEdges.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPointData.h"
#include "vtkPointDataToCellData.h"
#include "vtkPolyData.h"
#include "vtkTexturePackingFilter.h"

#include "vtkXMLPolyDataWriter.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkTextureAtlasGenerator);

namespace
{
//------------------------------------------------------------------------------
void PaintUV(vtkPolyData* mesh, vtkPolyData* texture)
{
  const double* bounds = texture->GetBounds();
  vtkNew<vtkFloatArray> uv;
  uv->SetNumberOfComponents(2);
  uv->SetNumberOfTuples(mesh->GetNumberOfPoints());
  uv->SetName(vtkTextureAtlasGenerator::UVCoordinatesArrayName());
  double p[3];
  for (vtkIdType pointId = 0; pointId < mesh->GetNumberOfPoints(); ++pointId)
  {
    texture->GetPoint(pointId, p);
    uv->SetTypedComponent(pointId, 0, (p[0] - bounds[0]) / (bounds[1] - bounds[0]));
    uv->SetTypedComponent(pointId, 1, (p[1] - bounds[2]) / (bounds[3] - bounds[2]));
  }
  mesh->GetPointData()->SetTCoords(uv);
}
} // anonymous namespace

//------------------------------------------------------------------------------
vtkTextureAtlasGenerator::vtkTextureAtlasGenerator()
{
  this->SetNumberOfOutputPorts(2);
}

//------------------------------------------------------------------------------
int vtkTextureAtlasGenerator::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfoMesh = outputVector->GetInformationObject(0);
  vtkInformation* outInfoAtlas = outputVector->GetInformationObject(1);

  vtkPolyData* input = vtkPolyData::GetData(inInfo);
  vtkPolyData* outputMesh = vtkPolyData::GetData(outInfoMesh);
  vtkPolyData* outputAtlas = vtkPolyData::GetData(outInfoAtlas);

  vtkNew<vtkGenerateGlobalIds> globalIds;
  globalIds->SetInputData(input);

  vtkNew<vtkNewFeatureEdges> featureEdges;
  featureEdges->SetInputConnection(globalIds->GetOutputPort());

  vtkNew<vtkDistanceToFeature> distanceToFeature;
  distanceToFeature->SetInputConnection(globalIds->GetOutputPort());
  distanceToFeature->SetSourceConnection(featureEdges->GetOutputPort());

  vtkNew<vtkPointDataToCellData> pd2cd;
  pd2cd->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDistanceToFeature::OutputArrayName());
  pd2cd->SetInputConnection(distanceToFeature->GetOutputPort());

  vtkNew<vtkGrowCharts> growCharts;
  growCharts->SetInputConnection(pd2cd->GetOutputPort());
  growCharts->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, vtkDistanceToFeature::OutputArrayName());

  vtkNew<vtkLSCMFilter> lscm;
  lscm->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, vtkGrowCharts::ChartIdArrayName());
  lscm->SetInputConnection(growCharts->GetOutputPort());

  vtkNew<vtkTexturePackingFilter> texturePacking;
  texturePacking->SetInputConnection(0, lscm->GetOutputPort(vtkLSCMFilter::ATLAS));
  texturePacking->SetInputConnection(1, lscm->GetOutputPort(vtkLSCMFilter::BOUNDARY));
  texturePacking->Update();

  vtkNew<vtkAppendDataSets> appendSegments;
  appendSegments->MergePointsOff();
  appendSegments->SetOutputDataSetType(VTK_POLY_DATA);
  auto pds = vtkPartitionedDataSet::SafeDownCast(lscm->GetSegmentsOutput());
  for (unsigned int i = 0; i < pds->GetNumberOfPartitions(); ++i)
  {
    appendSegments->AddInputData(pds->GetPartition(i));
  }
  appendSegments->Update();

  outputMesh->ShallowCopy(appendSegments->GetOutputDataObject(0));
  outputAtlas->ShallowCopy(texturePacking->GetOutputDataObject(0));

  PaintUV(outputMesh, vtkPolyData::SafeDownCast(texturePacking->GetOutputDataObject(0)));

  return 1;
}
VTK_ABI_NAMESPACE_END
