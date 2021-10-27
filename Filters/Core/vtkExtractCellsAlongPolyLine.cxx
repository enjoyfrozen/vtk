/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractCellsAlongPolyLine.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkExtractCellsAlongPolyLine.h"

#include "vtkArrayDispatch.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDataArrayRange.h"
#include "vtkDataSet.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkSMPTools.h"
#include "vtkStaticCellLocator.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

vtkStandardNewMacro(vtkExtractCellsAlongPolyLine);

namespace
{
//==============================================================================
// This struct is a toolset of elements needed by a vtkDataSet to extract the points of a given
// cell.
struct DataSetHelper
{
  using DataSetType = vtkDataSet;

  DataSetHelper(vtkDataSet* input)
    : Input(input)
  {
  }

  vtkDataSet* Input;
  vtkNew<vtkIdList> PointIds;
};

//==============================================================================
// This struct is a toolset of elements needed by a vtkUnstructuredGrid to extract the points
// of a given cell.
template <class ConnectivityArrayT>
struct UnstructuredGridHelper
{
  using ConnectivityArrayType = ConnectivityArrayT;
  using DataSetType = vtkUnstructuredGrid;

  UnstructuredGridHelper(vtkUnstructuredGrid* input)
    : Input(input)
  {
    vtkCellArray* cells = input->GetCells();
    this->Connectivity = vtkArrayDownCast<ConnectivityArrayType>(cells->GetConnectivityArray());
    this->Offsets = vtkArrayDownCast<ConnectivityArrayType>(cells->GetOffsetsArray());
  }

  vtkUnstructuredGrid* Input;
  ConnectivityArrayType* Connectivity;
  ConnectivityArrayType* Offsets;
};

//==============================================================================
// Helper class to read input cells depending on the input's type
// This class implements:
// * AddHitCellIdsAndPointIds: Given an input cell that is hit by an input line,
//   add the cell id in a cell id container and the points of the cell in a point id container,
//   and update the connectivty size of the output unstructured grid
// * CopyCell: Given an input input cell id, copy it into the output unstructured grid.
template <class DataSetT>
struct InputCellHandler;

//==============================================================================
template <>
struct InputCellHandler<vtkDataSet>
{
  static void AddHitCellIdsAndPointIds(vtkIdType cellId, const DataSetHelper& helper,
    vtkIdType& connectivitySize, std::unordered_set<vtkIdType>& intersectedCellIds,
    std::unordered_set<vtkIdType>& intersectedCellPointIds);

  template <class ArrayT>
  static void CopyCell(vtkIdType cellId, const DataSetHelper& helper,
    const std::unordered_map<vtkIdType, vtkIdType>& inputToOutputPointIdMap,
    vtkIdType currentOffset, ArrayT* outputConnectivity);
};

//==============================================================================
template <>
struct InputCellHandler<vtkUnstructuredGrid>
{
  template <class DataSetHelperT>
  static void AddHitCellIdsAndPointIds(vtkIdType cellId, const DataSetHelperT& helper,
    vtkIdType& connectivitySize, std::unordered_set<vtkIdType>& intersectedCellIds,
    std::unordered_set<vtkIdType>& intersectedCellPointIds);

  template <class DataSetHelperT, class ArrayT>
  static void CopyCell(vtkIdType cellId, const DataSetHelperT& helper,
    const std::unordered_map<vtkIdType, vtkIdType>& inputToOutputPointIdMap,
    vtkIdType currentOffset, ArrayT* outputConnectivity);
};

//------------------------------------------------------------------------------
void InputCellHandler<vtkDataSet>::AddHitCellIdsAndPointIds(vtkIdType cellId,
  const DataSetHelper& helper, vtkIdType& connectivitySize,
  std::unordered_set<vtkIdType>& intersectedCellIds,
  std::unordered_set<vtkIdType>& intersectedCellPointIds)
{
  vtkIdList* cellPointIds = helper.PointIds;
  helper.Input->GetCellPoints(cellId, cellPointIds);
  connectivitySize += intersectedCellIds.count(cellId) ? 0 : cellPointIds->GetNumberOfIds();
  intersectedCellIds.insert(cellId);

  for (vtkIdType cellPointId = 0; cellPointId < cellPointIds->GetNumberOfIds(); ++cellPointId)
  {
    intersectedCellPointIds.insert(cellPointIds->GetId(cellPointId));
  }
}

//------------------------------------------------------------------------------
template <class DataSetHelperT>
void InputCellHandler<vtkUnstructuredGrid>::AddHitCellIdsAndPointIds(vtkIdType cellId,
  const DataSetHelperT& helper, vtkIdType& connectivitySize,
  std::unordered_set<vtkIdType>& intersectedCellIds,
  std::unordered_set<vtkIdType>& intersectedCellPointIds)
{
  using ArrayType = typename DataSetHelperT::ConnectivityArrayType;

  ArrayType* offsets = helper.Offsets;
  ArrayType* connectivity = helper.Connectivity;

  vtkIdType startId = static_cast<vtkIdType>(offsets->GetValue(cellId));
  vtkIdType endId = static_cast<vtkIdType>(offsets->GetValue(cellId + 1));

  connectivitySize += intersectedCellIds.count(cellId) ? 0 : endId - startId;
  intersectedCellIds.insert(cellId);

  for (vtkIdType id = startId; id < endId; ++id)
  {
    intersectedCellPointIds.insert(connectivity->GetValue(id));
  }
}

//------------------------------------------------------------------------------
template <class ArrayT>
void InputCellHandler<vtkDataSet>::CopyCell(vtkIdType inputCellId, const DataSetHelper& helper,
  const std::unordered_map<vtkIdType, vtkIdType>& inputToOutputPointIdMap, vtkIdType currentOffset,
  ArrayT* outputConnectivity)
{
  vtkIdList* pointIds = helper.PointIds;
  helper.Input->GetCellPoints(inputCellId, pointIds);

  for (vtkIdType pointId = 0; pointId < pointIds->GetNumberOfIds(); ++pointId)
  {
    outputConnectivity->SetValue(
      currentOffset + pointId, inputToOutputPointIdMap.at(pointIds->GetId(pointId)));
  }
}

//------------------------------------------------------------------------------
template <class DataSetHelperT, class ArrayT>
void InputCellHandler<vtkUnstructuredGrid>::CopyCell(vtkIdType inputCellId,
  const DataSetHelperT& helper,
  const std::unordered_map<vtkIdType, vtkIdType>& inputToOutputPointIdMap, vtkIdType currentOffset,
  ArrayT* outputConnectivity)
{
  using ArrayType = typename DataSetHelperT::ConnectivityArrayType;

  ArrayType* connectivity = helper.Connectivity;
  ArrayType* offsets = helper.Offsets;

  vtkIdType startId = static_cast<vtkIdType>(offsets->GetValue(inputCellId));
  vtkIdType endId = static_cast<vtkIdType>(offsets->GetValue(inputCellId + 1));

  for (vtkIdType id = startId; id < endId; ++id)
  {
    outputConnectivity->SetValue(
      currentOffset + id - startId, inputToOutputPointIdMap.at(connectivity->GetValue(id)));
  }
}

//==============================================================================
template <class DataSetHelperT, class RangeT>
struct IntersectLinesWorker
{
  using DataSetType = typename DataSetHelperT::DataSetType;

  IntersectLinesWorker(DataSetType* input, vtkUnsignedCharArray* lineCellTypes,
    vtkPoints* linePoints, vtkAbstractCellLocator* locator, RangeT& connectivity, RangeT& offsets,
    vtkIdType& connectivitySize, std::unordered_set<vtkIdType>& intersectedCellIds,
    std::unordered_set<vtkIdType>& intersectedCellPointIds)
    : Input(input)
    , LineCellTypes(lineCellTypes)
    , LinePoints(linePoints)
    , Locator(locator)
    , Connectivity(connectivity)
    , Offsets(offsets)
    , GlobalIntersectedCellIds(intersectedCellIds)
    , GlobalIntersectedCellPointIds(intersectedCellPointIds)
    , GlobalConnectivitySize(connectivitySize)
  {
  }

  void Initialize() { this->ConnectivitySize.Local() = 0; }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    using ValueType = typename RangeT::value_type;

    DataSetHelperT helper(this->Input);
    double p1[3], p2[3];
    vtkNew<vtkIdList> cellIds;

    std::unordered_set<vtkIdType>& intersectedCellIds = this->IntersectedCellIds.Local();
    std::unordered_set<vtkIdType>& intersectedCellPointIds = this->IntersectedCellPointIds.Local();
    vtkIdType& connectivitySize = this->ConnectivitySize.Local();

    for (vtkIdType lineId = startId; lineId < endId; ++lineId)
    {
      ValueType start = this->Offsets[lineId];
      ValueType size = this->Offsets[lineId + 1] - start;

      if (this->LineCellTypes)
      {
        unsigned char cellType = this->LineCellTypes->GetValue(lineId);

        // We skip cells that are not lines
        if (cellType != VTK_LINE && cellType != VTK_POLY_LINE)
        {
          vtkLog(WARNING,
            "Cell at id " << lineId
                          << " in the source is not a vtkLine or a vtkPolyLine... Skipping.");
          continue;
        }
      }

      for (ValueType pointId = 0; pointId < size - 1; ++pointId)
      {
        this->LinePoints->GetPoint(this->Connectivity[start + pointId], p1);
        this->LinePoints->GetPoint(this->Connectivity[start + pointId + 1], p2);

        // tolerance is ignored in vtkStaticCellLocator
        this->Locator->FindCellsAlongLine(p1, p2, 0.0 /* tolerance */, cellIds);

        for (vtkIdType id = 0; id < cellIds->GetNumberOfIds(); ++id)
        {
          vtkIdType cellId = cellIds->GetId(id);
          InputCellHandler<DataSetType>::AddHitCellIdsAndPointIds(
            cellId, helper, connectivitySize, intersectedCellIds, intersectedCellPointIds);
        }
      }
    }
  }

  void Reduce()
  {
    for (const std::unordered_set<vtkIdType>& ids : this->IntersectedCellIds)
    {
      for (const vtkIdType& id : ids)
      {
        this->GlobalIntersectedCellIds.insert(id);
      }
    }
    for (const std::unordered_set<vtkIdType>& ids : this->IntersectedCellPointIds)
    {
      for (const vtkIdType& id : ids)
      {
        this->GlobalIntersectedCellPointIds.insert(id);
      }
    }
    for (vtkIdType size : this->ConnectivitySize)
    {
      this->GlobalConnectivitySize += size;
    }
  }

  DataSetType* Input;
  vtkUnsignedCharArray* LineCellTypes;
  vtkPoints* LinePoints;
  vtkAbstractCellLocator* Locator;
  RangeT& Connectivity;
  RangeT& Offsets;

  std::unordered_set<vtkIdType>& GlobalIntersectedCellIds;
  std::unordered_set<vtkIdType>& GlobalIntersectedCellPointIds;
  vtkIdType& GlobalConnectivitySize;

  vtkSMPThreadLocal<std::unordered_set<vtkIdType>> IntersectedCellIds;
  vtkSMPThreadLocal<std::unordered_set<vtkIdType>> IntersectedCellPointIds;
  vtkSMPThreadLocal<vtkIdType> ConnectivitySize;
};

//------------------------------------------------------------------------------
template <class DataSetHelperT, class RangeT>
void IntersectLines(typename DataSetHelperT::DataSetType* input, vtkCellArray* lineCells,
  vtkUnsignedCharArray* lineCellTypes, vtkPoints* linePoints, vtkAbstractCellLocator* locator,
  RangeT& connectivity, RangeT& offsets, vtkIdType& connectivitySize,
  std::unordered_set<vtkIdType>& intersectedCellIds,
  std::unordered_set<vtkIdType>& intersectedCellPointIds)
{
  IntersectLinesWorker<DataSetHelperT, RangeT> worker(input, lineCellTypes, linePoints, locator,
    connectivity, offsets, connectivitySize, intersectedCellIds, intersectedCellPointIds);

  vtkSMPTools::For(0, lineCells->GetNumberOfCells(), worker);
}

//==============================================================================
struct DataSetPointsCopyWorker
{
  DataSetPointsCopyWorker(vtkDataSet* input, vtkPoints* outputPoints, vtkIdList* pointIds)
    : Input(input)
    , OutputPoints(outputPoints)
    , PointIds(pointIds)
  {
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    double p[3];
    for (vtkIdType pointId = startId; pointId < endId; ++pointId)
    {
      this->Input->GetPoint(this->PointIds->GetId(pointId), p);
      this->OutputPoints->SetPoint(pointId, p);
    }
  }

  vtkDataSet* Input;
  vtkPoints* OutputPoints;
  vtkIdList* PointIds;
};

//==============================================================================
struct PointSetPointsCopyDispatcher
{
  template <class ArrayT1, class ArrayT2>
  void operator()(
    ArrayT1* source, ArrayT2* dest, vtkIdList* ids, vtkIdType startId, vtkIdType endId)
  {
    auto sourceRange = vtk::DataArrayTupleRange<3>(source);
    auto destRange = vtk::DataArrayTupleRange<3>(dest);
    using ConstSourceReference = typename decltype(sourceRange)::ConstTupleReferenceType;
    using DestReference = typename decltype(destRange)::TupleReferenceType;

    for (vtkIdType pointId = startId; pointId < endId; ++pointId)
    {
      ConstSourceReference sourceTuple = sourceRange[ids->GetId(pointId)];
      DestReference destTuple = destRange[pointId];
      std::copy(sourceTuple.begin(), sourceTuple.end(), destTuple.begin());
    }
  }
};

//==============================================================================
struct PointSetPointsCopyWorker
{
  PointSetPointsCopyWorker(vtkPoints* input, vtkPoints* output, vtkIdList* pointIds)
    : Input(input)
    , Output(output)
    , PointIds(pointIds)
  {
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    using Dispatch = vtkArrayDispatch::Dispatch2SameValueType;
    PointSetPointsCopyDispatcher dispatcher;
    Dispatch::Execute(
      this->Input->GetData(), this->Output->GetData(), dispatcher, this->PointIds, startId, endId);
  }

  vtkPoints* Input;
  vtkPoints* Output;
  vtkIdList* PointIds;
};

//==============================================================================
template <class ArrayT, class DataSetHelperT>
struct GenerateOutputCellsWorker
{
  using DataSetType = typename DataSetHelperT::DataSetType;

  GenerateOutputCellsWorker(vtkIdList* cellIds, DataSetType* input,
    const std::unordered_map<vtkIdType, vtkIdType>& inputToOutputPointIdMap,
    ArrayT* outputConnectivity, ArrayT* outputOffsets, vtkUnsignedCharArray* outputCellTypes)
    : CellIds(cellIds)
    , Input(input)
    , InputToOutputPointIdMap(inputToOutputPointIdMap)
    , OutputConnectivity(outputConnectivity)
    , OutputOffsets(outputOffsets)
    , OutputCellTypes(outputCellTypes)
  {
  }

  void operator()(vtkIdType startId, vtkIdType endId)
  {
    DataSetHelperT helper(this->Input);

    for (vtkIdType outputCellId = startId; outputCellId < endId; ++outputCellId)
    {
      vtkIdType inputCellId = this->CellIds->GetId(outputCellId);
      vtkIdType currentOffset = this->OutputOffsets->GetValue(outputCellId);

      InputCellHandler<DataSetType>::CopyCell(inputCellId, helper, this->InputToOutputPointIdMap,
        currentOffset, this->OutputConnectivity);

      this->OutputCellTypes->SetValue(outputCellId, this->Input->GetCellType(inputCellId));
    }
  }

  vtkIdList* CellIds;
  DataSetType* Input;
  const std::unordered_map<vtkIdType, vtkIdType>& InputToOutputPointIdMap;
  ArrayT* OutputConnectivity;
  ArrayT* OutputOffsets;
  vtkUnsignedCharArray* OutputCellTypes;
};

//------------------------------------------------------------------------------
template <class ArrayT, class DataSetHelperT>
void GenerateOutputCells(vtkIdList* cellIds,
  const std::unordered_map<vtkIdType, vtkIdType>& inputToOutputPointIdMap,
  typename DataSetHelperT::DataSetType* input, vtkIdType connectivitySize,
  vtkCellArray* outputCells, vtkUnsignedCharArray* outputCellTypes)
{
  vtkIdType numberOfOutputCells = cellIds->GetNumberOfIds();
  DataSetHelperT helper(input);

  auto outputConnectivity = vtkArrayDownCast<ArrayT>(outputCells->GetConnectivityArray());
  auto outputOffsets = vtkArrayDownCast<ArrayT>(outputCells->GetOffsetsArray());
  outputConnectivity->SetNumberOfValues(connectivitySize);
  outputOffsets->SetNumberOfValues(numberOfOutputCells + 1);
  outputOffsets->SetValue(0, 0);

  outputCellTypes->SetNumberOfValues(numberOfOutputCells);
  vtkIdType currentOffset = 0;

  for (vtkIdType outputCellId = 0; outputCellId < numberOfOutputCells; ++outputCellId)
  {
    currentOffset += input->GetCellSize(cellIds->GetId(outputCellId));
    outputOffsets->SetValue(outputCellId + 1, currentOffset);
  }

  GenerateOutputCellsWorker<ArrayT, DataSetHelperT> worker(
    cellIds, input, inputToOutputPointIdMap, outputConnectivity, outputOffsets, outputCellTypes);

  vtkSMPTools::For(0, numberOfOutputCells, worker);
}

//------------------------------------------------------------------------------
// This function extracts the cells in the input that are intersected by a set of input lines.
// The algorithm is as follows:
// * Intersect the lines using a cell locator and store their cell ids and point ids belonging to
//   each hit cell inside a std::unordered_set (for unicity).
// * Copy each unordered_set into a vtkIdList* and sort the ids to preserve the same ordering as in
//   the input.
//   Note: a std::unoredered_set is used as it is much faster than a std::set, even when calling
//   std::sort later on. Moreover, the more entropy there is in a container,
//   the faster std::sort is likely to be.
// * Copy input cells and points into output
template <class ArrayT>
int ExtractCells(vtkExtractCellsAlongPolyLine* self, vtkDataSet* input, vtkPointSet* lines,
  vtkUnstructuredGrid* output)
{
  using ArrayType32 = typename vtkCellArray::ArrayType32;
  using ArrayType64 = typename vtkCellArray::ArrayType64;

  vtkNew<vtkStaticCellLocator> locator;
  locator->SetDataSet(input);
  locator->BuildLocator();

  auto linesUG = vtkUnstructuredGrid::SafeDownCast(lines);
  auto linesPD = vtkPolyData::SafeDownCast(lines);

  if (!linesUG && !linesPD)
  {
    vtkLog(ERROR, "Input lines should be vtkUnstructuredGrid or vtkPolyData");
    return 0;
  }

  vtkPoints* linePoints = lines->GetPoints();

  std::unordered_set<vtkIdType> intersectedCellIds, intersectedCellPointIds;
  vtkIdType connectivitySize = 0;

  // This filter supports lines inside vtkPolyData as well as vtkUnstructuredGrid
  vtkCellArray* lineCells = linesPD ? linesPD->GetLines() : linesUG->GetCells();

  vtkUnsignedCharArray* lineCellTypes = linesUG ? linesUG->GetCellTypesArray() : nullptr;

  auto connectivity =
    vtk::DataArrayValueRange<1>(vtkArrayDownCast<ArrayT>(lineCells->GetConnectivityArray()));
  auto offsets =
    vtk::DataArrayValueRange<1>(vtkArrayDownCast<ArrayT>(lineCells->GetOffsetsArray()));

  auto inputUG = vtkUnstructuredGrid::SafeDownCast(input);

  // We take a fast path when input is an unstructured grid, as we can read the cell arrays directly
  // on intersected cells.
  if (inputUG)
  {
    vtkCellArray* cells = inputUG->GetCells();
    if (cells)
    {
      if (cells->IsStorage64Bit())
      {
        IntersectLines<UnstructuredGridHelper<ArrayType64>>(inputUG, lineCells, lineCellTypes,
          linePoints, locator, connectivity, offsets, connectivitySize, intersectedCellIds,
          intersectedCellPointIds);
      }
      else
      {
        IntersectLines<UnstructuredGridHelper<ArrayType32>>(inputUG, lineCells, lineCellTypes,
          linePoints, locator, connectivity, offsets, connectivitySize, intersectedCellIds,
          intersectedCellPointIds);
      }
    }
    else
    {
      // There are no cells to intersect.
      return 1;
    }
  }
  else
  {
    IntersectLines<DataSetHelper>(input, lineCells, lineCellTypes, linePoints, locator,
      connectivity, offsets, connectivitySize, intersectedCellIds, intersectedCellPointIds);
  }

  // Sorting cell ids and point ids
  vtkNew<vtkIdList> sortedIntersectedCellIds;
  sortedIntersectedCellIds->SetNumberOfIds(intersectedCellIds.size());
  std::copy(
    intersectedCellIds.cbegin(), intersectedCellIds.cend(), sortedIntersectedCellIds->begin());
  std::sort(sortedIntersectedCellIds->begin(), sortedIntersectedCellIds->end());

  vtkNew<vtkIdList> sortedIntersectedCellPointIds;
  sortedIntersectedCellPointIds->SetNumberOfIds(intersectedCellPointIds.size());
  std::copy(intersectedCellPointIds.cbegin(), intersectedCellPointIds.cend(),
    sortedIntersectedCellPointIds->begin());
  std::sort(sortedIntersectedCellPointIds->begin(), sortedIntersectedCellPointIds->end());

  std::unordered_map<vtkIdType, vtkIdType> inputToOutputPointIdMap;

  vtkIdType numberOfPoints = sortedIntersectedCellPointIds->GetNumberOfIds();
  vtkIdType numberOfCells = sortedIntersectedCellIds->GetNumberOfIds();

  // Mapping input cell ids to output cell ids
  for (vtkIdType pointId = 0; pointId < numberOfPoints; ++pointId)
  {
    inputToOutputPointIdMap.insert({ sortedIntersectedCellPointIds->GetId(pointId), pointId });
  }

  auto inputPS = vtkPointSet::SafeDownCast(input);
  vtkPoints* inputPoints = inputPS ? inputPS->GetPoints() : nullptr;

  // Handling of output points precision
  vtkNew<vtkPoints> points;
  if (inputPoints)
  {
    points->SetDataType(inputPoints->GetDataType());
  }
  else
  {
    switch (self->GetOutputPointsPrecision())
    {
      case vtkAlgorithm::DEFAULT_PRECISION:
      case vtkAlgorithm::SINGLE_PRECISION:
        points->SetDataType(VTK_FLOAT);
        break;
      case vtkAlgorithm::DOUBLE_PRECISION:
        points->SetDataType(VTK_DOUBLE);
        break;
      default:
        vtkLog(WARNING, "OutputPointsPrecision is not set to vtkAlgorithm::SINGLE_PRECISION"
            << " or vtkAlgorithn::DOUBLE_PRECISION");
        points->SetDataType(VTK_FLOAT);
        break;
    }
  }

  points->SetNumberOfPoints(numberOfPoints);
  output->SetPoints(points);

  if (inputPoints)
  {
    PointSetPointsCopyWorker worker(inputPoints, points, sortedIntersectedCellPointIds);
    vtkSMPTools::For(0, numberOfPoints, worker);
  }
  else
  {
    DataSetPointsCopyWorker worker(input, points, sortedIntersectedCellPointIds);
    vtkSMPTools::For(0, numberOfPoints, worker);
  }

  vtkNew<vtkCellArray> outputCells;
  vtkNew<vtkUnsignedCharArray> outputCellTypes;

#ifdef VTK_USE_64BIT_IDS
  if (!(numberOfPoints >> 32))
  {
    outputCells->ConvertTo32BitStorage();
  }
#endif

  // Copying input cells into output, compressing cell arrays if possible using 32 bits
  // Fast path is used if input is unstructured grid
  if (inputUG)
  {
    vtkCellArray* cells = inputUG->GetCells();
    if (cells)
    {
      int mask = static_cast<int>(cells->IsStorage64Bit()) | (outputCells->IsStorage64Bit() << 1);
      switch (mask)
      {
        case 0:
          GenerateOutputCells<ArrayType32, UnstructuredGridHelper<ArrayType32>>(
            sortedIntersectedCellIds, inputToOutputPointIdMap, inputUG, connectivitySize,
            outputCells, outputCellTypes);
          break;
        case 1:
          GenerateOutputCells<ArrayType32, UnstructuredGridHelper<ArrayType64>>(
            sortedIntersectedCellIds, inputToOutputPointIdMap, inputUG, connectivitySize,
            outputCells, outputCellTypes);
          break;
        case 2:
          GenerateOutputCells<ArrayType64, UnstructuredGridHelper<ArrayType32>>(
            sortedIntersectedCellIds, inputToOutputPointIdMap, inputUG, connectivitySize,
            outputCells, outputCellTypes);
          break;
        case 3:
          GenerateOutputCells<ArrayType64, UnstructuredGridHelper<ArrayType64>>(
            sortedIntersectedCellIds, inputToOutputPointIdMap, inputUG, connectivitySize,
            outputCells, outputCellTypes);
          break;
      }
    }
    else
    {
      // There are no cells to intersect.
      return 1;
    }
  }
  else
  {
    if (outputCells->IsStorage64Bit())
    {
      GenerateOutputCells<ArrayType64, DataSetHelper>(sortedIntersectedCellIds,
        inputToOutputPointIdMap, input, connectivitySize, outputCells, outputCellTypes);
    }
    else
    {
      GenerateOutputCells<ArrayType32, DataSetHelper>(sortedIntersectedCellIds,
        inputToOutputPointIdMap, input, connectivitySize, outputCells, outputCellTypes);
    }
  }

  output->SetCells(outputCellTypes, outputCells);

  // Copying point and cell data
  vtkCellData* inputCD = input->GetCellData();
  vtkCellData* outputCD = output->GetCellData();

  outputCD->CopyAllOn();
  outputCD->CopyAllocate(inputCD);
  outputCD->SetNumberOfTuples(numberOfCells);
  outputCD->CopyData(inputCD, sortedIntersectedCellIds);

  vtkPointData* inputPD = input->GetPointData();
  vtkPointData* outputPD = output->GetPointData();

  outputPD->CopyAllOn();
  outputPD->CopyAllocate(inputPD, numberOfPoints);
  outputPD->SetNumberOfTuples(numberOfPoints);
  outputPD->CopyData(inputPD, sortedIntersectedCellPointIds);

  return 1;
}
} // anonymous namespace

//------------------------------------------------------------------------------
vtkExtractCellsAlongPolyLine::vtkExtractCellsAlongPolyLine()
  : OutputPointsPrecision(vtkAlgorithm::DEFAULT_PRECISION)
{
  this->SetNumberOfInputPorts(2);
}

//------------------------------------------------------------------------------
vtkExtractCellsAlongPolyLine::~vtkExtractCellsAlongPolyLine() {}

//------------------------------------------------------------------------------
int vtkExtractCellsAlongPolyLine::RequestData(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // Check inputs / ouputs
  vtkInformation* inputInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* samplerInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  if (!outInfo || !inputInfo || !samplerInfo)
  {
    vtkErrorMacro("Missing input or output information");
    return 0;
  }

  auto input = vtkDataSet::SafeDownCast(inputInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto linesPS = vtkPointSet::SafeDownCast(samplerInfo->Get(vtkDataObject::DATA_OBJECT()));
  auto output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!output || !input || !linesPS)
  {
    vtkErrorMacro("Missing input or output");
    return 0;
  }

  using ArrayType32 = vtkCellArray::ArrayType32;
  using ArrayType64 = vtkCellArray::ArrayType64;

  vtkCellArray* cells;
  if (auto linesPD = vtkPolyData::SafeDownCast(linesPS))
  {
    cells = linesPD->GetLines();
  }
  else if (auto linesUG = vtkUnstructuredGrid::SafeDownCast(linesPS))
  {
    cells = linesUG->GetCells();
  }
  else
  {
    vtkErrorMacro("Unsupported source of type "
      << linesPS->GetClassName() << ". It should be a vtkPolyData or a vtkUnstructuredGrid.");
    return 0;
  }

  if (cells->IsStorage64Bit())
  {
    return ExtractCells<ArrayType64>(this, input, linesPS, output);
  }

  return ExtractCells<ArrayType32>(this, input, linesPS, output);
}

//------------------------------------------------------------------------------
int vtkExtractCellsAlongPolyLine::FillInputPortInformation(int port, vtkInformation* info)
{

  switch (port)
  {
    case 0:
      info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
      break;

    case 1:
      info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
      info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
      break;

    default:
      break;
  }

  return 1;
}

//------------------------------------------------------------------------------
void vtkExtractCellsAlongPolyLine::SetSourceConnection(vtkAlgorithmOutput* input)
{
  this->SetInputConnection(1, input);
}

//------------------------------------------------------------------------------
void vtkExtractCellsAlongPolyLine::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
