/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointSetToOctreeImageFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPointSetToOctreeImageFilter.h"

#include "vtkArrayDispatch.h"
#include "vtkAtomicMutex.h"
#include "vtkCellData.h"
#include "vtkDataArrayRange.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkSMPTools.h"
#include "vtkUnsignedCharArray.h"

#include <mutex>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkPointSetToOctreeImageFilter);

//------------------------------------------------------------------------------
vtkPointSetToOctreeImageFilter::vtkPointSetToOctreeImageFilter()
{
  this->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
}

//------------------------------------------------------------------------------
vtkPointSetToOctreeImageFilter::~vtkPointSetToOctreeImageFilter() = default;

//------------------------------------------------------------------------------
void vtkPointSetToOctreeImageFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Size: " << this->Size << endl;
  os << indent << "ProcessInputPointArray: " << this->ProcessInputPointArray << endl;
  os << indent << "ComputeLastValue: " << this->ComputeLastValue << endl;
  os << indent << "ComputeMin: " << this->ComputeMin << endl;
  os << indent << "ComputeMax: " << this->ComputeMax << endl;
  os << indent << "ComputeCount: " << this->ComputeCount << endl;
  os << indent << "ComputeSum: " << this->ComputeSum << endl;
  os << indent << "ComputeMean: " << this->ComputeMean << endl;
}

//------------------------------------------------------------------------------
int vtkPointSetToOctreeImageFilter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
  return 1;
}

//------------------------------------------------------------------------------
template <typename TPointsArray>
struct vtkPointSetToOctreeImageFilter::PointSetToImageFunctor
{
  vtkPointSet* Input;
  vtkImageData* Output;
  TPointsArray* Points;
  vtkUnsignedCharArray* Octree;
  vtkDataArray* InField;
  vtkFloatArray* OutField;
  const std::vector<FieldFunctions>& Functions;

  bool UseFieldArray;
  double Origin[3];
  double Spacing[3];
  double Spacing_2[3];
  int Dimensions[3];
  int UpperExtent[3];

  std::unique_ptr<vtkAtomicMutex[]> Locks;

  PointSetToImageFunctor(vtkPointSet* input, vtkImageData* output, TPointsArray* points,
    vtkUnsignedCharArray* octree, vtkDataArray* inField, vtkFloatArray* outField,
    const std::vector<FieldFunctions>& functions)
    : Input(input)
    , Output(output)
    , Points(points)
    , Octree(octree)
    , InField(inField)
    , OutField(outField)
    , Functions(functions)
    , UseFieldArray(inField != nullptr)
  {
    output->GetOrigin(this->Origin);
    output->GetSpacing(this->Spacing);
    this->Spacing_2[0] = 0.5 * this->Spacing[0];
    this->Spacing_2[1] = 0.5 * this->Spacing[1];
    this->Spacing_2[2] = 0.5 * this->Spacing[2];
    output->GetDimensions(this->Dimensions);
    this->UpperExtent[0] = this->Dimensions[0] - 1;
    this->UpperExtent[1] = this->Dimensions[1] - 1;
    this->UpperExtent[2] = this->Dimensions[2] - 1;

    this->Locks =
      std::unique_ptr<vtkAtomicMutex[]>(new vtkAtomicMutex[this->Output->GetNumberOfCells()]);
  }

  void Initialize() {}

  void operator()(vtkIdType begin, vtkIdType end)
  {
    double* origin = this->Origin;
    double* spacing = this->Spacing;
    double* spacing_2 = this->Spacing_2;
    int* dimensions = this->Dimensions;
    int* upperExtent = this->UpperExtent;
    vtkImageData* output = this->Output;
    int numFunctions = this->Functions.size();
    // no need to iterate over the mean function since it will be computed in the reduce step
    if (this->UseFieldArray && this->Functions.back() == FieldFunctions::MEAN)
    {
      --numFunctions;
    }

    double outPt[3];
    int ijk[3];
    vtkIdType outPtId;
    vtkIdType outCellId;
    unsigned char octreeValue;
    float inFieldValue;

    const auto inPoints = vtk::DataArrayTupleRange<3>(this->Points);
    unsigned char* octree = this->Octree->GetPointer(0);

    vtk::detail::ValueRange<vtkDataArray, 1> inField;
    vtk::detail::TupleRange<vtkAOSDataArrayTemplate<float>, vtk::detail::DynamicTupleSize> outField;
    if (this->UseFieldArray)
    {
      inField = vtk::DataArrayValueRange<1>(this->InField);
      outField = vtk::DataArrayTupleRange(this->OutField);
    }
    for (vtkIdType i = begin; i < end; ++i)
    {
      // get point
      const auto& inPt = inPoints[i];
      // calculate output point and cell id
      ijk[0] = static_cast<int>(std::floor((inPt[0] - origin[0]) / spacing[0]));
      ijk[1] = static_cast<int>(std::floor((inPt[1] - origin[1]) / spacing[0]));
      ijk[2] = static_cast<int>(std::floor((inPt[2] - origin[2]) / spacing[0]));
      outPtId = ijk[0] + ijk[1] * dimensions[0] + ijk[2] * dimensions[0] * dimensions[1];
      outCellId = ijk[0] + ijk[1] * upperExtent[0] + ijk[2] * upperExtent[0] * upperExtent[1];
      // get output point
      output->GetPoint(outPtId, outPt);
      // add half spacing to output point
      outPt[0] += spacing_2[0];
      outPt[1] += spacing_2[1];
      outPt[2] += spacing_2[2];
      // calculate scalar octree value
      octreeValue = inPt[0] > outPt[0] ? 2 : 1;
      octreeValue *= inPt[1] > outPt[1] ? 4 : 1;
      octreeValue *= inPt[2] > outPt[2] ? 16 : 1;

      std::lock_guard<vtkAtomicMutex> lock(this->Locks[outCellId]);
      // this operation is deterministic because we perform a bitwise OR
      octree[outCellId] |= octreeValue;

      if (this->UseFieldArray)
      {
        auto outTuple = outField[outCellId];
        inFieldValue = static_cast<float>(inField[i]);
        for (int j = 0; j < numFunctions; ++j)
        {
          switch (this->Functions[j])
          {
            case FieldFunctions::LAST_VALUE:
              outTuple[j] = inFieldValue;
              break;
            case FieldFunctions::MIN:
              outTuple[j] = std::min(static_cast<float>(outTuple[j]), inFieldValue);
              break;
            case FieldFunctions::MAX:
              outTuple[j] = std::max(static_cast<float>(outTuple[j]), inFieldValue);
              break;
            case FieldFunctions::COUNT:
              outTuple[j] += 1.0f;
              break;
            case FieldFunctions::SUM:
              outTuple[j] += inFieldValue;
              break;
            default:
              break;
          }
        }
      }
    }
  }

  void Reduce()
  {
    // Compute mean
    if (this->UseFieldArray && this->Functions.back() == FieldFunctions::MEAN)
    {
      vtkSMPTools::For(0, this->OutField->GetNumberOfTuples(), [&](vtkIdType begin, vtkIdType end) {
        auto outField = vtk::DataArrayTupleRange(this->OutField, begin, end);
        const int numFunctions = static_cast<int>(this->Functions.size());
        const int meanIndex = numFunctions - 1;
        const int sumIndex = numFunctions - 2;
        const int countIndex = numFunctions - 3;
        for (auto tuple : outField)
        {
          if (tuple[countIndex] != 0.0f)
          {
            tuple[meanIndex] = static_cast<float>(tuple[sumIndex]) / tuple[countIndex];
          }
        }
      });
    }
  }
};

//------------------------------------------------------------------------------
struct vtkPointSetToOctreeImageFilter::PointSetToImageWorker
{
  template <typename TPointsArray>
  void operator()(TPointsArray* inPointsArray, vtkUnsignedCharArray* octreeArray,
    vtkPointSet* input, vtkImageData* output, vtkDataArray* inField, vtkFloatArray* outField,
    const std::vector<FieldFunctions>& functions)
  {
    PointSetToImageFunctor<TPointsArray> functor(
      input, output, inPointsArray, octreeArray, inField, outField, functions);
    vtkSMPTools::For(0, input->GetNumberOfPoints(), functor);
  }
};

//------------------------------------------------------------------------------
int vtkPointSetToOctreeImageFilter::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the input
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkPointSet* input = vtkPointSet::GetData(inInfo);

  // get the output
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkPartitionedDataSet* outputPDS = vtkPartitionedDataSet::GetData(outInfo);

  if (!input || input->GetNumberOfPoints() == 0)
  {
    vtkErrorMacro("No input or empty input.");
    return 0;
  }

  // get input points information
  double bounds[6];
  input->GetBounds(bounds);
  const double origin[3] = { bounds[0], bounds[2], bounds[4] };
  const double distance[3] = { bounds[1] - bounds[0], bounds[3] - bounds[2],
    bounds[5] - bounds[4] };

  // compute output image information
  const double volume = distance[0] * distance[1] * distance[2];
  const double spacing = std::pow(volume / this->Size, 1.0 / 3.0);
  const int dimensions[3] = { 1 + static_cast<int>(std::ceil(distance[0] / spacing)),
    1 + static_cast<int>(std::ceil(distance[1] / spacing)),
    1 + static_cast<int>(std::ceil(distance[2] / spacing)) };
  const vtkIdType numberOfCells = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);

  // create output image octree array
  vtkNew<vtkUnsignedCharArray> octree;
  octree->SetName("octree");
  octree->SetNumberOfValues(numberOfCells);
  vtkSMPTools::Fill(octree->GetPointer(0), octree->GetPointer(0) + numberOfCells, 0);

  // create output image field array
  vtkSmartPointer<vtkFloatArray> outField = nullptr;
  vtkDataArray* inField = nullptr;
  std::vector<FieldFunctions> functions;
  if (this->ProcessInputPointArray)
  {
    inField = this->GetInputArrayToProcess(0, inputVector);
    if (!inField)
    {
      vtkErrorMacro("Array to process is null.");
      return 0;
    }
    if (inField->GetNumberOfTuples() != input->GetNumberOfPoints())
    {
      vtkErrorMacro("Array to process must have as many tuples as the number of points.");
      return 0;
    }
    if (inField->GetNumberOfComponents() != 1)
    {
      vtkErrorMacro("Array to process '" << inField->GetName() << "' must have 1 component.");
      return 0;
    }
    int numberOfFunctions = this->ComputeLastValue + this->ComputeMin + this->ComputeMax +
      (this->ComputeCount || this->ComputeMean) + (this->ComputeSum || this->ComputeMean) +
      this->ComputeMean;
    if (numberOfFunctions == 0)
    {
      vtkErrorMacro("No function has been requested to be computed.");
      return 0;
    }
    functions.resize(static_cast<size_t>(numberOfFunctions));
    outField = vtkSmartPointer<vtkFloatArray>::New();
    outField->SetName(inField->GetName());
    outField->SetNumberOfComponents(numberOfFunctions);
    int counter = 0;
    if (this->ComputeLastValue)
    {
      functions[counter] = FieldFunctions::LAST_VALUE;
      outField->SetComponentName(counter++, "LastValue");
    }
    if (this->ComputeMin)
    {
      functions[counter] = FieldFunctions::MIN;
      outField->SetComponentName(counter++, "Min");
    }
    if (this->ComputeMax)
    {
      functions[counter] = FieldFunctions::MAX;
      outField->SetComponentName(counter++, "Max");
    }
    if (this->ComputeCount || this->ComputeMean)
    {
      functions[counter] = FieldFunctions::COUNT;
      outField->SetComponentName(counter++, "Count");
    }
    if (this->ComputeSum || this->ComputeMean)
    {
      functions[counter] = FieldFunctions::SUM;
      outField->SetComponentName(counter++, "Sum");
    }
    if (this->ComputeMean)
    {
      functions[counter] = FieldFunctions::MEAN;
      outField->SetComponentName(counter++, "Mean");
    }
    outField->SetNumberOfTuples(numberOfCells);

    // initialize output image field array
    std::vector<float> defaultValues(functions.size());
    for (int i = 0; i < numberOfFunctions; ++i)
    {
      switch (functions[i])
      {
        case FieldFunctions::LAST_VALUE:
          defaultValues[i] = 0.0f;
          break;
        case FieldFunctions::MIN:
          defaultValues[i] = std::numeric_limits<float>::max();
          break;
        case FieldFunctions::MAX:
          defaultValues[i] = std::numeric_limits<float>::lowest();
          break;
        case FieldFunctions::COUNT:
          defaultValues[i] = 0;
        case FieldFunctions::SUM:
          defaultValues[i] = 0;
          break;
        case FieldFunctions::MEAN:
          defaultValues[i] = 0;
          break;
      }
    }
    vtkSMPTools::For(0, numberOfCells, [&](vtkIdType begin, vtkIdType end) {
      auto outFieldRange = vtk::DataArrayTupleRange(outField, begin, end);
      for (auto outTuple : outFieldRange)
      {
        std::copy(defaultValues.begin(), defaultValues.end(), outTuple.begin());
      }
    });
  }

  // define output image
  vtkNew<vtkImageData> output;
  output->SetDimensions(dimensions);
  output->SetOrigin(origin);
  output->SetSpacing(spacing, spacing, spacing);
  output->GetCellData()->SetScalars(octree);
  if (inField)
  {
    output->GetCellData()->AddArray(outField);
  }

  // add output image to output partitioned dataset
  outputPDS->SetNumberOfPartitions(1);
  outputPDS->SetPartition(0, output);

  // fill octree and field arrays
  auto inPointsArray = input->GetPoints()->GetData();
  PointSetToImageWorker worker;
  using Dispatcher = vtkArrayDispatch::DispatchByValueType<vtkArrayDispatch::Reals>;
  if (!Dispatcher::Execute(
        inPointsArray, worker, octree, input, output, inField, outField.Get(), functions))
  {
    worker(inPointsArray, octree, input, output, inField, outField.Get(), functions);
  }

  return 1;
}
