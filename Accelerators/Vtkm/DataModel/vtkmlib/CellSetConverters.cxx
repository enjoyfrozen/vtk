// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-FileCopyrightText: Copyright (c) Kitware, Inc.
// SPDX-FileCopyrightText: Copyright 2012 Sandia Corporation.
// SPDX-License-Identifier: LicenseRef-BSD-3-Clause-Sandia-USGov
#include "CellSetConverters.h"

#include "ArrayConverters.hxx"
#include "DataSetConverters.h"

#include <vtkm/cont/openmp/DeviceAdapterOpenMP.h>
#include <vtkm/cont/serial/DeviceAdapterSerial.h>
#include <vtkm/cont/tbb/DeviceAdapterTBB.h>

#include <vtkm/cont/Algorithm.h>
#include <vtkm/cont/ArrayCopy.h>
#include <vtkm/cont/ArrayHandleCast.h>
#include <vtkm/cont/ArrayHandleGroupVec.h>
#include <vtkm/cont/ArrayHandleTransform.h>
#include <vtkm/cont/CellSetExplicit.h>
#include <vtkm/cont/CellSetSingleType.h>
#include <vtkm/cont/TryExecute.h>

#include <vtkm/worklet/WorkletMapField.h>

#include <vtkm/BinaryPredicates.h>
#include <vtkm/Swap.h>

#include "vtkCellArray.h"
#include "vtkCellType.h"
#include "vtkDataArrayRange.h"
#include "vtkIdTypeArray.h"
#include "vtkNew.h"
#include "vtkUnsignedCharArray.h"
#include "vtkmDataArray.h"

namespace tovtkm
{
VTK_ABI_NAMESPACE_BEGIN

namespace
{

struct ReorderHex : vtkm::worklet::WorkletMapField
{
  using ControlSignature = void(FieldInOut);

  VTKM_EXEC void operator()(vtkm::Vec<vtkm::Id, 8>& indices) const
  {
    vtkm::Swap(indices[2], indices[3]);
    vtkm::Swap(indices[6], indices[7]);
  }
};

#define SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(arrayCls, dataType)                        \
  if (auto asConcreteArray = arrayCls<dataType>::SafeDownCast(connectivity))                       \
  {                                                                                                \
    constexpr bool IsVtkmIdType = std::is_same<dataType, vtkm::Id>::value;                         \
    using DirectConverter = tovtkm::DataArrayToArrayHandle<arrayCls<dataType>, 1>;                 \
    auto connHandleDirect = DirectConverter::Wrap(asConcreteArray);                                \
    auto connHandle = IsVtkmIdType ? connHandleDirect                                              \
                                   : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(connHandleDirect); \
    using ConnHandleType = typename std::decay<decltype(connHandle)>::type;                        \
    using ConnectivityStorageTag = typename ConnHandleType::StorageTag;                            \
    using CellSetType = vtkm::cont::CellSetSingleType<ConnectivityStorageTag>;                     \
    CellSetType cellSet;                                                                           \
    cellSet.Fill(static_cast<vtkm::Id>(numPoints), cellType, cellSize, connHandle);                \
    return cellSet;                                                                                \
  }

struct BuildSingleTypeCellSetVisitor
{
  template <typename CellStateT>
  vtkm::cont::UnknownCellSet operator()(
    CellStateT& state, vtkm::UInt8 cellType, vtkm::IdComponent cellSize, vtkIdType numPoints)
  {
    auto connectivity = state.GetConnectivity();
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::UInt8);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::UInt16);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::UInt32);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::UInt64);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::Int8);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::Int16);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::Int32);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkmDataArray, vtkm::Int64);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::UInt8);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::UInt16);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::UInt32);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::UInt64);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::Int8);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::Int16);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::Int32);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkAOSDataArrayTemplate, vtkm::Int64);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::UInt8);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::UInt16);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::UInt32);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::UInt64);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::Int8);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::Int16);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::Int32);
    SINGLE_TYPE_CELLSET_FROM_VTK_GENERIC_DATA_ARRAY(vtkSOADataArrayTemplate, vtkm::Int64);
    {
      // Fallback if none of the above worked.
      // Construct an arrayhandle that holds the connectivity array
      auto connRange = state.GetConnectivityRange();
      auto connHandleDirect = vtkm::cont::make_ArrayHandle(
        reinterpret_cast<vtkm::Id*>(connRange.data()), connRange.size(), vtkm::CopyFlag::Off);
      using ConnHandleType = vtkm::cont::ArrayHandle<typename CellStateT::ConnectivityValueType>;
      using ConnectivityStorageTag = typename ConnHandleType::StorageTag;
      using CellSetType = vtkm::cont::CellSetSingleType<ConnectivityStorageTag>;
      CellSetType cellSet;
      cellSet.Fill(static_cast<vtkm::Id>(numPoints), cellType, cellSize, connHandleDirect);
      return cellSet;
    }
  }
};

struct BuildSingleTypeVoxelCellSetVisitor
{
  template <typename CellStateT>
  vtkm::cont::UnknownCellSet operator()(CellStateT& state, vtkIdType numPoints)
  {
    using VTKIdT = typename CellStateT::ConnectivityValueType; // might not be vtkIdType...
    if (auto connAsVtkmArray = vtkmDataArray<VTKIdT>::SafeDownCast(state.GetConnectivity()))
    {
      vtkm::cont::ArrayHandle<vtkm::Id> connHandle;
      {
        vtkm::cont::ArrayCopy(connAsVtkmArray->GetVtkmUnknownArrayHandle(), connHandle);
        // reorder cells from voxel->hex
        vtkm::cont::Invoker invoke;
        invoke(ReorderHex{}, vtkm::cont::make_ArrayHandleGroupVec<8>(connHandle));
      }

      using CellSetType = vtkm::cont::CellSetSingleType<>;

      CellSetType cellSet;
      cellSet.Fill(numPoints, vtkm::CELL_SHAPE_HEXAHEDRON, 8, connHandle);
      return cellSet;
    }
    else
    {
      vtkm::cont::ArrayHandle<vtkm::Id> connHandle;
      {
        auto range = state.GetConnectivityRange();
        vtkm::cont::ArrayCopy(
          vtkm::cont::make_ArrayHandle<VTKIdT>(
            reinterpret_cast<VTKIdT*>(range.data()), range.size(), vtkm::CopyFlag::Off),
          connHandle);

        // reorder cells from voxel->hex
        vtkm::cont::Invoker invoke;
        invoke(ReorderHex{}, vtkm::cont::make_ArrayHandleGroupVec<8>(connHandle));
      }

      using CellSetType = vtkm::cont::CellSetSingleType<>;

      CellSetType cellSet;
      cellSet.Fill(numPoints, vtkm::CELL_SHAPE_HEXAHEDRON, 8, connHandle);
      return cellSet;
    }
  }
};

} // end anon namespace

// convert a cell array of a single type to a vtkm CellSetSingleType
vtkm::cont::UnknownCellSet ConvertSingleType(
  vtkCellArray* cells, int cellType, vtkIdType numberOfPoints)
{
  switch (cellType)
  {
    case VTK_LINE:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_LINE, 2, numberOfPoints);

    case VTK_HEXAHEDRON:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_HEXAHEDRON, 8, numberOfPoints);

    case VTK_VOXEL:
      // Note that this is a special case that reorders ids voxel to hex:
      return cells->Visit(BuildSingleTypeVoxelCellSetVisitor{}, numberOfPoints);

    case VTK_QUAD:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_QUAD, 4, numberOfPoints);

    case VTK_TETRA:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_TETRA, 4, numberOfPoints);

    case VTK_TRIANGLE:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_TRIANGLE, 3, numberOfPoints);

    case VTK_VERTEX:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_VERTEX, 1, numberOfPoints);

    case VTK_WEDGE:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_WEDGE, 6, numberOfPoints);

    case VTK_PYRAMID:
      return cells->Visit(
        BuildSingleTypeCellSetVisitor{}, vtkm::CELL_SHAPE_PYRAMID, 5, numberOfPoints);

    default:
      break;
  }

  throw vtkm::cont::ErrorBadType("Unsupported VTK cell type in "
                                 "CellSetSingleType converter.");
}

namespace
{

struct BuildExplicitCellSetVisitor
{
  template <typename CellStateT, typename S>
  vtkm::cont::UnknownCellSet operator()(CellStateT& state,
    const vtkm::cont::ArrayHandle<vtkm::UInt8, S>& shapes, vtkm::Id numPoints) const
  {
    using ConnectivityValueType =
      typename CellStateT::ConnectivityValueType;                   // might not be vtkIdType...
    using OffsetsValueType = typename CellStateT::OffsetsValueType; // might not be vtkIdType...
    static constexpr bool ConnIsVtkmIdType = std::is_same<ConnectivityValueType, vtkm::Id>::value;
    static constexpr bool OffstIsVtkmIdType = std::is_same<OffsetsValueType, vtkm::Id>::value;

    if (auto connAsVtkmArray =
          vtkmDataArray<ConnectivityValueType>::SafeDownCast(state.GetConnectivity()))
    {
      if (auto offsetsAsVtkmArray =
            vtkmDataArray<OffsetsValueType>::SafeDownCast(state.GetOffsets()))
      {
        // Construct arrayhandles to hold the arrays
        using OffstDirectConverter =
          tovtkm::DataArrayToArrayHandle<vtkmDataArray<OffsetsValueType>, 1>;
        using ConnDirectConverter =
          tovtkm::DataArrayToArrayHandle<vtkmDataArray<ConnectivityValueType>, 1>;
        auto offsetsHandleDirect = OffstDirectConverter::Wrap(offsetsAsVtkmArray);
        auto connHandleDirect = ConnDirectConverter::Wrap(connAsVtkmArray);

        // Cast if necessary:
        auto connHandle = ConnIsVtkmIdType
          ? connHandleDirect
          : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(connHandleDirect);
        auto offsetsHandle = OffstIsVtkmIdType
          ? offsetsHandleDirect
          : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(offsetsHandleDirect);

        using ConnectivityStorageTag = typename decltype(connHandle)::StorageTag;
        using OffsetsStorageTag = typename decltype(offsetsHandle)::StorageTag;
        using CellSetType =
          vtkm::cont::CellSetExplicit<S, ConnectivityStorageTag, OffsetsStorageTag>;

        CellSetType cellSet;
        cellSet.Fill(numPoints, shapes, connHandle, offsetsHandle);
        return cellSet;
      }
    }
    else if (auto connAsAosArray = vtkAOSDataArrayTemplate<ConnectivityValueType>::SafeDownCast(
               state.GetConnectivity()))
    {
      if (auto offsetsAsAosArray =
            vtkAOSDataArrayTemplate<OffsetsValueType>::SafeDownCast(state.GetOffsets()))
      {
        // Construct arrayhandles to hold the arrays
        using OffstDirectConverter =
          tovtkm::DataArrayToArrayHandle<vtkAOSDataArrayTemplate<OffsetsValueType>, 1>;
        using ConnDirectConverter =
          tovtkm::DataArrayToArrayHandle<vtkAOSDataArrayTemplate<ConnectivityValueType>, 1>;
        auto offsetsHandleDirect = OffstDirectConverter::Wrap(offsetsAsAosArray);
        auto connHandleDirect = ConnDirectConverter::Wrap(connAsAosArray);

        // Cast if necessary:
        auto connHandle = ConnIsVtkmIdType
          ? connHandleDirect
          : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(connHandleDirect);
        auto offsetsHandle = OffstIsVtkmIdType
          ? offsetsHandleDirect
          : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(offsetsHandleDirect);

        using ConnectivityStorageTag = typename decltype(connHandle)::StorageTag;
        using OffsetsStorageTag = typename decltype(offsetsHandle)::StorageTag;
        using CellSetType =
          vtkm::cont::CellSetExplicit<S, ConnectivityStorageTag, OffsetsStorageTag>;

        CellSetType cellSet;
        cellSet.Fill(numPoints, shapes, connHandle, offsetsHandle);
        return cellSet;
      }
    }
    else if (auto connAsSoaArray = vtkSOADataArrayTemplate<ConnectivityValueType>::SafeDownCast(
               state.GetConnectivity()))
    {
      if (auto offsetsAsSoaArray =
            vtkSOADataArrayTemplate<OffsetsValueType>::SafeDownCast(state.GetOffsets()))
      {
        // Construct arrayhandles to hold the arrays
        using OffstDirectConverter =
          tovtkm::DataArrayToArrayHandle<vtkSOADataArrayTemplate<OffsetsValueType>, 1>;
        using ConnDirectConverter =
          tovtkm::DataArrayToArrayHandle<vtkSOADataArrayTemplate<ConnectivityValueType>, 1>;
        auto offsetsHandleDirect = OffstDirectConverter::Wrap(offsetsAsSoaArray);
        auto connHandleDirect = ConnDirectConverter::Wrap(connAsSoaArray);

        // Cast if necessary:
        auto connHandle = ConnIsVtkmIdType
          ? connHandleDirect
          : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(connHandleDirect);
        auto offsetsHandle = OffstIsVtkmIdType
          ? offsetsHandleDirect
          : vtkm::cont::make_ArrayHandleCast<vtkm::Id>(offsetsHandleDirect);

        using ConnectivityStorageTag = typename decltype(connHandle)::StorageTag;
        using OffsetsStorageTag = typename decltype(offsetsHandle)::StorageTag;
        using CellSetType =
          vtkm::cont::CellSetExplicit<S, ConnectivityStorageTag, OffsetsStorageTag>;

        CellSetType cellSet;
        cellSet.Fill(numPoints, shapes, connHandle, offsetsHandle);
        return cellSet;
      }
    }
    // Fallback to vtkDataArray
    // Construct arrayhandles to hold the arrays
    auto offsetsRange = state.GetOffsetsRange();
    auto connRange = state.GetConnectivityRange();
    auto offsetsHandleDirect = vtkm::cont::make_ArrayHandle(
      reinterpret_cast<vtkm::Id*>(offsetsRange.data()), offsetsRange.size(), vtkm::CopyFlag::Off);
    auto connHandleDirect = vtkm::cont::make_ArrayHandle(
      reinterpret_cast<vtkm::Id*>(connRange.data()), offsetsRange.size(), vtkm::CopyFlag::Off);

    using ConnectivityStorageTag = typename decltype(connHandleDirect)::StorageTag;
    using OffsetsStorageTag = typename decltype(offsetsHandleDirect)::StorageTag;
    using CellSetType = vtkm::cont::CellSetExplicit<S, ConnectivityStorageTag, OffsetsStorageTag>;

    CellSetType cellSet;
    cellSet.Fill(numPoints, shapes, connHandleDirect, offsetsHandleDirect);
    return cellSet;
  }
};

struct SupportedCellShape
{
  VTKM_EXEC_CONT
  bool operator()(vtkm::UInt8 shape) const
  {
    return (shape < vtkm::NUMBER_OF_CELL_SHAPES) && (shape != 2) && (shape != 6) && (shape != 8) &&
      (shape != 11);
  }
};

} // end anon namespace

// convert a cell array of mixed types to a vtkm CellSetExplicit
vtkm::cont::UnknownCellSet Convert(
  vtkUnsignedCharArray* types, vtkCellArray* cells, vtkIdType numberOfPoints)
{
  auto shapes = tovtkm::vtkAOSDataArrayToFlatArrayHandle(types);
  if (!vtkm::cont::Algorithm::Reduce(
        vtkm::cont::make_ArrayHandleTransform(shapes, SupportedCellShape{}), true,
        vtkm::LogicalAnd()))
  {
    throw vtkm::cont::ErrorBadType("Unsupported VTK cell type in CellSet converter.");
  }

  return cells->Visit(BuildExplicitCellSetVisitor{}, shapes, numberOfPoints);
}

VTK_ABI_NAMESPACE_END
} // namespace tovtkm

namespace fromvtkm
{
VTK_ABI_NAMESPACE_BEGIN

bool Convert(const vtkm::cont::UnknownCellSet& toConvert, vtkCellArray* cells,
  vtkUnsignedCharArray* typesArray)
{
  const auto* cellset = toConvert.GetCellSetBase();
  const vtkm::Id numCells = cellset->GetNumberOfCells();

  if (toConvert.CanConvert<vtkm::cont::CellSetSingleType<>>())
  {
    vtkm::cont::CellSetSingleType<> single;
    toConvert.AsCellSet(single);
    auto connectivity = vtk::TakeSmartPointer(make_vtkmDataArray(single.GetConnectivityArray(
      vtkm::TopologyElementTagCell(), vtkm::TopologyElementTagPoint())));
    cells->SetData(single.GetNumberOfPointsInCell(0), connectivity);
    if (typesArray != nullptr)
    {
      typesArray->SetNumberOfComponents(1);
      typesArray->SetNumberOfTuples(static_cast<vtkIdType>(numCells));
      for (vtkm::Id cellId = 0; cellId < numCells; ++cellId)
      {
        const vtkIdType vtkCellId = static_cast<vtkIdType>(cellId);
        typesArray->SetValue(vtkCellId, cellset->GetCellShape(cellId));
      }
    }
    return true;
  }
  else if (toConvert.CanConvert<vtkm::cont::CellSetExplicit<>>())
  {
    vtkm::cont::CellSetExplicit<> explicitCS;
    toConvert.AsCellSet(explicitCS);
    auto connectivity = vtk::TakeSmartPointer(make_vtkmDataArray(explicitCS.GetConnectivityArray(
      vtkm::TopologyElementTagCell(), vtkm::TopologyElementTagPoint())));
    auto offsets = vtk::TakeSmartPointer(make_vtkmDataArray(
      explicitCS.GetOffsetsArray(vtkm::TopologyElementTagCell(), vtkm::TopologyElementTagPoint())));
    cells->SetData(offsets, connectivity);
    auto shapesArray =
      explicitCS.GetShapesArray(vtkm::TopologyElementTagCell(), vtkm::TopologyElementTagPoint());
    if (typesArray != nullptr)
    {
      typesArray->SetNumberOfComponents(1);
      typesArray->SetNumberOfTuples(static_cast<vtkIdType>(numCells));
      for (vtkm::Id cellId = 0; cellId < numCells; ++cellId)
      {
        const vtkIdType vtkCellId = static_cast<vtkIdType>(cellId);
        typesArray->SetValue(vtkCellId, cellset->GetCellShape(cellId));
      }
    }
    return true;
  }
  vtkGenericWarningMacro(<< "Zero copy in fromvtkm::Convert failed!");
  // small hack as we can't compute properly the number of cells
  // instead we will pre-allocate and than shrink
  const vtkm::Id maxSize = numCells * 8; // largest cell type is hex

  // TODO this could steal the guts out of explicit cellsets as a future
  // no-copy optimization.
  vtkNew<vtkIdTypeArray> offsetsArray;
  offsetsArray->SetNumberOfTuples(static_cast<vtkIdType>(numCells + 1));
  vtkNew<vtkIdTypeArray> connArray;
  connArray->SetNumberOfTuples(static_cast<vtkIdType>(maxSize));

  if (typesArray)
  {
    typesArray->SetNumberOfComponents(1);
    typesArray->SetNumberOfTuples(static_cast<vtkIdType>(numCells));
  }

  vtkIdType* connIter = connArray->GetPointer(0);
  const vtkIdType* connBegin = connIter;

  for (vtkm::Id cellId = 0; cellId < numCells; ++cellId)
  {
    const vtkIdType vtkCellId = static_cast<vtkIdType>(cellId);
    const vtkm::Id npts = cellset->GetNumberOfPointsInCell(cellId);
    assert(npts <= 8 && "Initial allocation assumes no more than 8 pts/cell.");

    const vtkIdType offset = static_cast<vtkIdType>(connIter - connBegin);
    offsetsArray->SetValue(vtkCellId, offset);

    cellset->GetCellPointIds(cellId, connIter);
    connIter += npts;

    if (typesArray)
    {
      typesArray->SetValue(vtkCellId, cellset->GetCellShape(cellId));
    }
  }

  const vtkIdType connSize = static_cast<vtkIdType>(connIter - connBegin);
  offsetsArray->SetValue(static_cast<vtkIdType>(numCells), connSize);
  connArray->Resize(connSize);
  cells->SetData(offsetsArray, connArray);

  return true;
}

VTK_ABI_NAMESPACE_END
} // namespace fromvtkm
