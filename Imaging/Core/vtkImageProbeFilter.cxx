/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageProbeFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImageProbeFilter.h"

#include "vtkBoundingBox.h"
#include "vtkCellData.h"
#include "vtkCharArray.h"
#include "vtkIdTypeArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkSMPThreadLocal.h"
#include "vtkSMPTools.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <algorithm>
#include <vector>

vtkStandardNewMacro(vtkImageProbeFilter);

class vtkImageProbeFilter::vtkVectorOfArrays : public std::vector<vtkDataArray*>
{
};

//----------------------------------------------------------------------------
vtkImageProbeFilter::vtkImageProbeFilter()
{
  this->ValidPoints = vtkIdTypeArray::New();
  this->MaskPoints = nullptr;
  this->SetNumberOfInputPorts(2);
  this->ValidPointMaskArrayName = nullptr;
  this->SetValidPointMaskArrayName("vtkValidPointMask");
  this->CellArrays = new vtkVectorOfArrays();

  this->PointList = nullptr;
  this->CellList = nullptr;

  this->PassCellArrays = 0;
  this->PassPointArrays = 0;
  this->PassFieldArrays = 1;
  this->Tolerance = 1.0;
  this->ComputeTolerance = 1;
}

//----------------------------------------------------------------------------
vtkImageProbeFilter::~vtkImageProbeFilter()
{
  if (this->MaskPoints)
  {
    this->MaskPoints->Delete();
  }
  this->ValidPoints->Delete();

  this->SetValidPointMaskArrayName(nullptr);

  delete this->CellArrays;
  delete this->PointList;
  delete this->CellList;
}

//----------------------------------------------------------------------------
int vtkImageProbeFilter::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 1)
  {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  }
  else
  {
    info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataSet");
  }
  return 1;
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::SetSourceConnection(vtkAlgorithmOutput* algOutput)
{
  this->SetInputConnection(1, algOutput);
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::SetSourceData(vtkDataObject* input)
{
  this->SetInputData(1, input);
}

//----------------------------------------------------------------------------
vtkDataObject* vtkImageProbeFilter::GetSource()
{
  if (this->GetNumberOfInputConnections(1) < 1)
  {
    return nullptr;
  }

  return this->GetExecutive()->GetInputData(1, 0);
}

//----------------------------------------------------------------------------
vtkIdTypeArray* vtkImageProbeFilter::GetValidPoints()
{
  if (this->MaskPoints && this->MaskPoints->GetMTime() > this->ValidPoints->GetMTime())
  {
    char* maskArray = this->MaskPoints->GetPointer(0);
    vtkIdType numPts = this->MaskPoints->GetNumberOfTuples();
    vtkIdType numValidPoints = std::count(maskArray, maskArray + numPts, static_cast<char>(1));
    this->ValidPoints->Allocate(numValidPoints);
    for (vtkIdType i = 0; i < numPts; ++i)
    {
      if (maskArray[i])
      {
        this->ValidPoints->InsertNextValue(i);
      }
    }
    this->ValidPoints->Modified();
  }

  return this->ValidPoints;
}

//----------------------------------------------------------------------------
int vtkImageProbeFilter::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkDataSet* input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkImageData* source = vtkImageData::SafeDownCast(sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet* output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // copy the geometry of the Input to the Output
  output->CopyStructure(input);

  // probe the Source to generate the Output attributes
  if (source)
  {
    this->Probe(input, source, output);
  }

  // if any attributes weren't probed, copy them from Input to Output
  this->PassAttributeData(input, output);

  return 1;
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::PassAttributeData(vtkDataSet* input, vtkDataSet* output)
{
  // copy point data arrays
  if (this->PassPointArrays)
  {
    int numPtArrays = input->GetPointData()->GetNumberOfArrays();
    for (int i = 0; i < numPtArrays; ++i)
    {
      vtkDataArray* da = input->GetPointData()->GetArray(i);
      if (!output->GetPointData()->HasArray(da->GetName()))
      {
        output->GetPointData()->AddArray(da);
      }
    }

    // Set active attributes in the output to the active attributes in the input
    for (int i = 0; i < vtkDataSetAttributes::NUM_ATTRIBUTES; ++i)
    {
      vtkAbstractArray* da = input->GetPointData()->GetAttribute(i);
      if (da && da->GetName() && !output->GetPointData()->GetAttribute(i))
      {
        output->GetPointData()->SetAttribute(da, i);
      }
    }
  }

  // copy cell data arrays
  if (this->PassCellArrays)
  {
    int numCellArrays = input->GetCellData()->GetNumberOfArrays();
    for (int i = 0; i < numCellArrays; ++i)
    {
      vtkDataArray* da = input->GetCellData()->GetArray(i);
      if (!output->GetCellData()->HasArray(da->GetName()))
      {
        output->GetCellData()->AddArray(da);
      }
    }

    // Set active attributes in the output to the active attributes in the input
    for (int i = 0; i < vtkDataSetAttributes::NUM_ATTRIBUTES; ++i)
    {
      vtkAbstractArray* da = input->GetCellData()->GetAttribute(i);
      if (da && da->GetName() && !output->GetCellData()->GetAttribute(i))
      {
        output->GetCellData()->SetAttribute(da, i);
      }
    }
  }

  if (this->PassFieldArrays)
  {
    // nothing to do, vtkDemandDrivenPipeline takes care of that.
  }
  else
  {
    output->GetFieldData()->Initialize();
  }
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::BuildFieldList(vtkImageData* source)
{
  // delete leftovers from previous execution
  delete this->PointList;
  delete this->CellList;

  // initialize from Source attributes
  this->PointList = new vtkDataSetAttributes::FieldList(1);
  this->PointList->InitializeFieldList(source->GetPointData());

  this->CellList = new vtkDataSetAttributes::FieldList(1);
  this->CellList->InitializeFieldList(source->GetCellData());
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::InitializeForProbing(vtkDataSet* input, vtkDataSet* output)
{
  if (!this->PointList || !this->CellList)
  {
    vtkErrorMacro("BuildFieldList() must be called before calling this method.");
    return;
  }

  vtkIdType numPts = input->GetNumberOfPoints();

  // if this is repeatedly called by the pipeline for a composite mesh,
  // you need a new array for each block
  // (that is you need to reinitialize the object)
  if (this->MaskPoints)
  {
    this->MaskPoints->Delete();
  }
  this->MaskPoints = vtkCharArray::New();
  this->MaskPoints->SetNumberOfComponents(1);
  this->MaskPoints->SetNumberOfTuples(numPts);
  this->MaskPoints->FillValue(0);
  this->MaskPoints->SetName(
    this->ValidPointMaskArrayName ? this->ValidPointMaskArrayName : "vtkValidPointMask");

  // Allocate storage for output PointData
  // All input PD is passed to output as PD. Those arrays in input CD that are
  // not present in output PD will be passed as output PD.
  vtkPointData* outPD = output->GetPointData();
  outPD->InterpolateAllocate((*this->PointList), numPts, numPts);

  vtkCellData* tempCellData = vtkCellData::New();
  // We're okay with copying global ids for cells. we just don't flag them as
  // such.
  tempCellData->CopyAllOn(vtkDataSetAttributes::COPYTUPLE);
  tempCellData->CopyAllocate((*this->CellList), numPts, numPts);

  this->CellArrays->clear();
  int numCellArrays = tempCellData->GetNumberOfArrays();
  for (int cc = 0; cc < numCellArrays; cc++)
  {
    vtkDataArray* inArray = tempCellData->GetArray(cc);
    if (inArray && inArray->GetName() && !outPD->GetArray(inArray->GetName()))
    {
      outPD->AddArray(inArray);
      this->CellArrays->push_back(inArray);
    }
  }
  tempCellData->Delete();

  this->InitializeOutputArrays(outPD, numPts);
  outPD->AddArray(this->MaskPoints);
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::InitializeOutputArrays(vtkPointData* outPD, vtkIdType numPts)
{
  for (int i = 0; i < outPD->GetNumberOfArrays(); ++i)
  {
    vtkDataArray* da = outPD->GetArray(i);
    if (da)
    {
      da->SetNumberOfTuples(numPts);
      da->Fill(0);
    }
  }
}

//----------------------------------------------------------------------------
namespace
{

// Thread local storage
struct ProbePointsThreadLocal
{
  ProbePointsThreadLocal()
  {
    // BaseThread will be set 'true' for the thread that gets the first piece
    this->BaseThread = false;
    this->PointIds = vtkSmartPointer<vtkIdList>::New();
    this->PointIds->SetNumberOfIds(8);
  }

  bool BaseThread;
  vtkSmartPointer<vtkIdList> PointIds;
};

}

//----------------------------------------------------------------------------
class vtkImageProbeFilter::ProbePointsWorklet
{
public:
  ProbePointsWorklet(vtkImageProbeFilter* probeFilter, vtkDataSet* input, vtkImageData* source,
    int srcIdx, vtkPointData* outPD, char* maskArray)
    : ProbeFilter(probeFilter)
    , Input(input)
    , Source(source)
    , BlockId(srcIdx)
    , OutPointData(outPD)
    , MaskArray(maskArray)
  {
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    if (startId == 0)
    {
      this->Thread.Local().BaseThread = true;
    }
    this->ProbeFilter->ProbePoints(this->Input, this->Source, this->BlockId, this->OutPointData,
      this->MaskArray, this->Thread.Local().PointIds.GetPointer(), startId, endId,
      this->Thread.Local().BaseThread);
  }

private:
  vtkImageProbeFilter* ProbeFilter;
  vtkDataSet* Input;
  vtkImageData* Source;
  int BlockId;
  vtkPointData* OutPointData;
  char* MaskArray;
  vtkSMPThreadLocal<ProbePointsThreadLocal> Thread;
};

//----------------------------------------------------------------------------
void vtkImageProbeFilter::DoProbing(
  vtkDataSet* input, int srcIdx, vtkImageData* source, vtkDataSet* output)
{
  vtkBoundingBox sbox(source->GetBounds());
  vtkBoundingBox ibox(input->GetBounds());
  if (!sbox.Intersects(ibox))
  {
    return;
  }

  vtkDebugMacro(<< "Probing data");

  vtkPointData* outPD = output->GetPointData();
  char* maskArray = this->MaskPoints->GetPointer(0);

  // Estimate the granularity for multithreading
  int threads = vtkSMPTools::GetEstimatedNumberOfThreads();
  vtkIdType numPts = input->GetNumberOfPoints();
  vtkIdType grain = numPts / threads;
  vtkIdType minGrain = 1000;
  vtkIdType maxGrain = 1000;
  grain = vtkMath::ClampValue(grain, minGrain, maxGrain);

  // Multithread the execution
  ProbePointsWorklet worklet(this, input, source, srcIdx, outPD, maskArray);
  vtkSMPTools::For(0, numPts, grain, worklet);

  this->MaskPoints->Modified();
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::Probe(vtkDataSet* input, vtkImageData* source, vtkDataSet* output)
{
  // first stage of attribute initialization: initialize from Source
  this->BuildFieldList(source);

  // second stage of attribute initialization: initialize from Input
  this->InitializeForProbing(input, output);

  // probe the Source at each Input point
  this->DoProbing(input, 0, source, output);
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::ProbePoints(vtkDataSet* input, vtkImageData* source, int srcIdx,
  vtkPointData* outPD, char* maskArray, vtkIdList* pointIds, vtkIdType startId, vtkIdType endId,
  bool baseThread)
{
  vtkPointData* pd = source->GetPointData();
  vtkCellData* cd = source->GetCellData();

  // Get image information
  double spacing[3], origin[3];
  source->GetOrigin(origin);
  source->GetSpacing(spacing);
  int extent[6];
  source->GetExtent(extent);
  vtkIdType increments[3];
  source->GetIncrements(increments);

  const double relativeTol = 1e-3;
  double tol = this->Tolerance;
  if (this->ComputeTolerance)
  {
    // Compute tolerance based on smallest voxel dimension
    double l = std::min(spacing[0], spacing[1]);
    l = std::min(l, spacing[2]);
    tol = l * relativeTol;
  }

  // Convert extent to double for use in comparisons
  double boundCheck[6];
  for (int i = 0; i < 3; i++)
  {
    boundCheck[2 * i] = extent[2 * i] - tol / spacing[i];
    boundCheck[2 * i + 1] = extent[2 * i + 1] + tol / spacing[i];
  }

  // Loop over all input points, interpolating source data
  vtkIdType progressInterval = endId / 20 + 1;
  for (vtkIdType ptId = startId; ptId < endId && !GetAbortExecute(); ptId++)
  {
    if (baseThread && !(ptId % progressInterval))
    {
      // This is not ideal, because if the base thread executes more than one piece,
      // then the progress will repeat its 0.0 to 1.0 progression for each piece.
      this->UpdateProgress(static_cast<double>(ptId) / endId);
    }

    if (maskArray[ptId] == static_cast<char>(1))
    {
      // skip points which have already been probed with success.
      // This is helpful for multiblock dataset probing.
      continue;
    }

    // Get the xyz coordinate of the point in the input dataset
    double x[3];
    input->GetPoint(ptId, x);

    // Convert to structured coordinates
    for (int i = 0; i < 3; i++)
    {
      x[i] = (x[i] - origin[i]) / spacing[i];
    }

    // Do bounds check (includes tolerance)
    if (x[0] >= boundCheck[0] && x[0] <= boundCheck[1] && x[1] >= boundCheck[2] &&
      x[1] <= boundCheck[3] && x[2] >= boundCheck[4] && x[2] <= boundCheck[5])
    {
      // Initialize weights and point Ids for interpolation
      double weights[8];
      vtkIdType* ptIds = pointIds->GetPointer(0);
      for (int i = 0; i < 8; i++)
      {
        weights[i] = 1.0;
        ptIds[i] = 0;
      }

      // Initialize cellId (will compute within loop)
      vtkIdType cellId = 0;
      vtkIdType cellIncrement = 1;

      // Loop through dimensions
      for (int i = 0; i < 3; i++)
      {
        // Check for reduced dimensionality
        bool hasWidth = (extent[2 * i] != extent[2 * i + 1]);

        // Compute the index required for interpolation
        int idx = vtkMath::Floor(x[i]);

        // Limit the index to the extent
        if (idx < extent[2 * i])
        {
          idx = extent[2 * i];
        }
        else if (idx >= extent[2 * i + 1])
        {
          idx = extent[2 * i + 1] - hasWidth;
        }

        // Incrementally compute the cellId (needed for cell attributes)
        int idxAdjusted = idx - extent[2 * i];
        cellId += cellIncrement * idxAdjusted;
        cellIncrement *= extent[2 * i + 1] - extent[2 * i] - hasWidth;

        // Incrementally compute linear interpolation coefficients
        double f = x[i] - idx;
        double r = 1.0 - f;
        int c = 1 << i;
        for (int j = 0; j < 4; j++)
        {
          int a = j + (j >> i) * c;
          int b = a + c;
          weights[a] *= r;
          weights[b] *= f;
          ptIds[a] += increments[i] * (idxAdjusted);
          ptIds[b] += increments[i] * (idxAdjusted + hasWidth);
        }
      }

      // Interpolate the point data
      outPD->InterpolatePoint((*this->PointList), pd, srcIdx, ptId, pointIds, weights);

      // Copy the cell data
      for (vtkVectorOfArrays::iterator iter = this->CellArrays->begin();
           iter != this->CellArrays->end(); ++iter)
      {
        vtkDataArray* inArray = cd->GetArray((*iter)->GetName());
        if (inArray)
        {
          outPD->CopyTuple(inArray, *iter, cellId, ptId);
        }
      }
      maskArray[ptId] = static_cast<char>(1);
    }
  }

  this->MaskPoints->Modified();
}

//----------------------------------------------------------------------------
int vtkImageProbeFilter::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // copy extent info from Input to Output
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
    inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()), 6);

  // copy scalar info from Source to Output
  int scalarType = vtkImageData::GetScalarType(sourceInfo);
  int numComponents = vtkImageData::GetNumberOfScalarComponents(sourceInfo);
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, scalarType, numComponents);

  return 1;
}

//----------------------------------------------------------------------------
int vtkImageProbeFilter::RequestUpdateExtent(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // copy update extent from Output to Input
  inInfo->Set(vtkStreamingDemandDrivenPipeline::EXACT_EXTENT(), 1);
  inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT()), 6);

  // set update extent of Source to its whole extent
  sourceInfo->Remove(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT());
  if (sourceInfo->Has(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()))
  {
    sourceInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
      sourceInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()), 6);
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkImageProbeFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkDataObject* source = this->GetSource();

  this->Superclass::PrintSelf(os, indent);
  os << indent << "Source: " << source << "\n";
  os << indent << "ValidPointMaskArrayName: "
     << (this->ValidPointMaskArrayName ? this->ValidPointMaskArrayName : "vtkValidPointMask")
     << "\n";
  os << indent << "PassFieldArrays: " << (this->PassFieldArrays ? "On" : " Off") << "\n";
}
