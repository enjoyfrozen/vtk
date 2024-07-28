#include "vtkDGOperation.h"

#include "vtkCellAttribute.h"
#include "vtkCellGrid.h"
#include "vtkDGCell.h"
#include "vtkDoubleArray.h"
#include "vtkMatrix3x3.h"

VTK_ABI_NAMESPACE_BEGIN

using namespace vtk::literals;

namespace
{

/// Find the evaluator that matches the given \a cellId.
vtkDGOperation::EvaluatorMap::const_iterator FindEvaluator(
  vtkDGOperation::RangeKey cellKey, const vtkDGOperation::EvaluatorMap& evaluators)
{
  auto cellId = cellKey.Begin;
  auto it = evaluators.lower_bound(cellKey);
  if (it == evaluators.end())
  {
    // Either \a cellId is past the end of cell-ids covered by \a evaluators
    // or the final entry in evaluators covers \a cellId.
    if (evaluators.empty()) { return it; }
    const auto& lastKey(evaluators.rbegin()->first);
    if (lastKey.Contains(cellId))
    {
      // cellId > lastKey.Begin but cellId < lastKey.End.
      return evaluators.find(lastKey);
    }
    // cellId >= lastKey.End
    return it;
  }
  // If it->first.Begin == cellId, we have a match:
  if (it->first.Contains(cellId))
  {
    return it;
  }
  if (it == evaluators.begin())
  {
    // The first entry of evaluators doesn't contain \a cellId but
    // \a cellId >= it->first.Begin and \a cellId < (it++)->first.Begin.
    // This should only be possible if there is a gap between ranges of cell-ids
    // covered by \a evaluators. This should not happen.
    return evaluators.end();
  }
  // it->first.Begin >= \a cellId. Back up to see if the previous evaluator
  // entry contains \a cellId.
  --it;
  if (it->first.Contains(cellId))
  {
    return it;
  }
  return evaluators.end();
}

} // anonymous namespace

bool vtkDGOperation::RangeKey::Contains(vtkTypeUInt64 cellId) const
{
  return cellId >= this->Begin && cellId < this->End;
}

bool vtkDGOperation::RangeKey::ContainedBy(const RangeKey& other) const
{
  return other.Begin >= this->Begin && other.End < this->Begin;
}

bool vtkDGOperation::RangeKey::operator < (const RangeKey& other) const
{
  return this->Begin < other.Begin;
}

vtkDGOperation::EvaluatorEntry& vtkDGOperation::EvaluatorEntry::operator = (
  vtkDGOperation::EvaluatorEntry const& other)
{
  other.State->CloneInto(*this);
  return *this;
}

/// Whether degrees of freedom are shared between cells.
enum SharingType
{
  SharedDOF,    //!< Degrees of freedom (DOF) are shared.
  Discontinuous //!< Degrees of freedom are not shared.
};

/// Whether cells are stand-alone or sides of other cells.
enum SideType
{
  Cells,
  Sides
};

/// Which type of shape-function post-processing is required.
enum ShapeModifier
{
  InverseJacobian, ///!< For HCURL
  None, ///!< For HGRAD
  ScaledJacobian ///!< For HDIV
};

template<SharingType DOFSharing, SideType SourceType, ShapeModifier Modifier, SharingType ShapeSharing = Discontinuous>
class OpEval : public vtkDGOperation::EvaluationState
{
public:
#if 0
  vtkDGOperatorEntry OpEntry;
  vtkDataArray* CellConnectivity;
  vtkDataArray* CellValues;
  vtkDataArray* SideConnectivity;
  vtkTypeUInt64 Offset;
  mutable std::array<vtkTypeUInt64, 2> SideTuple;
  mutable std::array<double, 3> RST{{ 0, 0, 0 }};
  mutable std::vector<vtkTypeUInt64> ConnTuple;
  mutable std::vector<double> ValueTuple;
  mutable std::vector<double> BasisTuple;
  mutable vtkTypeUInt64 LastCellId{ ~0ULL };
  mutable int NumberOfValuesPerFunction{ 0 };

  vtkDGOperatorEntry ShapeGradientEntry;
  vtkDataArray* ShapeConnectivity;
  vtkDataArray* ShapeValues;
  mutable std::vector<vtkTypeUInt64> ShapeConnTuple;
  mutable std::vector<double> ShapeValueTuple;
  mutable std::vector<double> ShapeBasisTuple;
  mutable std::vector<double> Jacobian;
  mutable int NumberOfShapeValuesPerFunction{ 0 };
  mutable vtkTypeUInt64 LastShapeCellId{ ~0ULL };
#endif

  OpEval(
    // Attribute arrays/operation
    vtkDGOperatorEntry& op,
    vtkDataArray* connectivity,
    vtkDataArray* values,
    vtkDataArray* sideConn,
    vtkTypeUInt64 offset,
    // Shape arrays/operation
    vtkDGOperatorEntry shapeGradient = vtkDGOperatorEntry(),
    vtkDataArray* shapeConnectivity = nullptr,
    vtkDataArray* shapeValues = nullptr)
    : EvaluationState(op, connectivity, values, sideConn, offset, shapeGradient, shapeConnectivity, shapeValues)
  {
    if (!op)
    {
      throw std::logic_error("Must have non-null operator.");
    }
    if (Modifier != None && !shapeGradient)
    {
      throw std::logic_error("Must have non-null shape gradient operator.");
    }
    this->BasisTuple.resize(op.NumberOfFunctions * op.OperatorSize);
    int ncc = 0;
    if (this->CellConnectivity)
    {
      ncc = this->CellConnectivity->GetNumberOfComponents();
      this->ConnTuple.resize(ncc);
    }
    else if (DOFSharing == SharedDOF)
    {
      throw std::logic_error("DOF sharing requires a cell-connectivity array.");
    }
    int nvc = this->CellValues->GetNumberOfComponents();
    if (DOFSharing == SharedDOF)
    {
      this->NumberOfValuesPerFunction = nvc;
      this->ValueTuple.resize(nvc * ncc);
    }
    else
    {
      this->NumberOfValuesPerFunction = nvc / this->OpEntry.NumberOfFunctions;
      this->ValueTuple.resize(nvc);
    }

    // If we must also evaluate the shape-attribute modifier for each
    // result value, then prepare tuples to hold shape data.
    if (Modifier != None)
    {
      this->Jacobian.resize(9); // TODO: Handle 2-d Jacobians differently eventually.
      this->ShapeBasisTuple.resize(shapeGradient.NumberOfFunctions * shapeGradient.OperatorSize);
      int nsc = 0;
      if (this->ShapeConnectivity)
      {
        nsc = this->ShapeConnectivity->GetNumberOfComponents();
        this->ShapeConnTuple.resize(nsc);
      }
      else if (ShapeSharing == SharedDOF)
      {
        throw std::logic_error("Shape DOF-sharing requires a shape-connectivity array.");
      }
      int nvs = this->ShapeValues->GetNumberOfComponents();
      if (ShapeSharing == SharedDOF)
      {
        this->NumberOfShapeValuesPerFunction = nvs;
        this->ShapeValueTuple.resize(nvs * nsc);
      }
      else
      {
        this->NumberOfShapeValuesPerFunction = nvs / this->ShapeGradientEntry.NumberOfFunctions;
        this->ShapeValueTuple.resize(nvs);
      }
    }
  }

  void CloneInto(vtkDGOperation::EvaluatorEntry& entry) const override;

  /// Compute the inner product of this->BasisTuple and this->ValueTuple, storing
  /// the result in the \a tt-th tuple of \a result.
  void InnerProduct(vtkTypeUInt64 tt, vtkDoubleArray* result) const
  {
    int nc = result->GetNumberOfComponents();
    double* xx = result->GetPointer(tt * nc);
    // Zero out the tuple:
    for (int ii = 0; ii < nc; ++ii)
    {
      xx[ii] = 0.;
    }
    for (int ii = 0; ii < this->NumberOfValuesPerFunction; ++ii)
    {
      for (int jj = 0; jj < this->OpEntry.OperatorSize; ++jj)
      {
        for (int kk = 0; kk < this->OpEntry.NumberOfFunctions; ++kk)
        {
          xx[ii * this->OpEntry.OperatorSize + jj] +=
            this->BasisTuple[kk * this->OpEntry.OperatorSize + jj] *
            this->ValueTuple[kk * this->NumberOfValuesPerFunction + ii];
        }
      }
    }
  }

  /// Compute the inner product of this->ShapeBasisTuple and this->ShapeValueTuple, storing
  /// the result in this->Jacobian.
  void ShapeInnerProduct() const
  {
    int nc = 9; // TODO: Use cell dimension instead (9 for 3-d, 4 for 2-d)?
    // Zero out the tuple:
    for (int ii = 0; ii < nc; ++ii)
    {
      this->Jacobian[ii] = 0.;
    }
    for (int ii = 0; ii < this->NumberOfShapeValuesPerFunction; ++ii)
    {
      for (int jj = 0; jj < this->ShapeGradientEntry.OperatorSize; ++jj)
      {
        for (int kk = 0; kk < this->ShapeGradientEntry.NumberOfFunctions; ++kk)
        {
          this->Jacobian[jj + this->NumberOfShapeValuesPerFunction * ii] +=
            this->ShapeBasisTuple[kk * this->ShapeGradientEntry.OperatorSize + jj] *
            this->ShapeValueTuple[kk * this->NumberOfShapeValuesPerFunction + ii];
        }
      }
    }
  }

  ///  Compute the shape-attribute Jacobian matrix, storing it in this->Jacobian.
  void ComputeJacobian() const
  {
    if (ShapeSharing == SharedDOF)
    {
      if (this->LastShapeCellId != this->LastCellId)
      {
        this->ShapeConnectivity->GetUnsignedTuple(this->LastCellId, this->ShapeConnTuple.data());
        std::size_t nc = this->ShapeConnTuple.size();
        int nv = this->ShapeValues->GetNumberOfComponents();
        for (std::size_t jj = 0; jj < nc; ++jj)
        {
          this->ShapeValues->GetTuple(this->ShapeConnTuple[jj], this->ShapeValueTuple.data() + nv * jj);
        }
        this->LastShapeCellId = this->LastCellId;
      }
    }
    else if (ShapeSharing == Discontinuous)
    {
      if (this->LastShapeCellId != this->LastCellId)
      {
        this->ShapeValues->GetTuple(this->LastCellId, this->ShapeValueTuple.data());
        this->LastShapeCellId = this->LastCellId;
      }
    }
    else
    {
      throw std::logic_error("Invalid shape DOF-sharing enumerant.");
    }
    this->ShapeGradientEntry.Op(this->RST, this->ShapeBasisTuple);
    this->ShapeInnerProduct();
  }

  // Compute the inverse Jacobian and multiply the \a ii-th tuple of result by it.
  //
  // This performs the multiplication in place.
  void ApplyInverseJacobian(vtkTypeUInt64 ii, vtkDoubleArray* result) const
  {
    this->ComputeJacobian();
    // Invert Jacobian and multiply result's ii-th tuple by it.
    std::array<double, 9> inverseJacobian;
    vtkMatrix3x3::Invert(this->Jacobian.data(), inverseJacobian.data());
    std::array<double, 3> vec;
    double* rr = result->GetPointer(0);
    const int nc = result->GetNumberOfComponents();
    if (nc % 3 != 0)
    {
      throw std::logic_error("Jacobian must apply to vector or matrix values.");
    }
    const vtkTypeUInt64 nn = ii * nc;
    for (int vv = 0; vv < nc / 3; ++vv)
    {
      vtkTypeUInt64 mm = nn + 3 * vv;
      vtkMatrix3x3::MultiplyPoint(inverseJacobian.data(), rr + mm, rr + mm);
    }
  }

  // Compute the Jacobian scaled by its determinant and multiply the \a ii-th tuple of result by it.
  //
  // This performs the multiplication in place.
  void ApplyScaledJacobian(vtkTypeUInt64 ii, vtkDoubleArray* result) const
  {
    this->ComputeJacobian();
    // Compute the Jacobian's determinant and multiply result's ii-th tuple
    // by both the Jacobian and the scalar determinant.
    double det = vtkMatrix3x3::Determinant(this->Jacobian.data());
    std::array<double, 3> vec;
    double* rr = result->GetPointer(0);
    const int nc = result->GetNumberOfComponents();
    if (nc % 3 != 0)
    {
      throw std::logic_error("Jacobian must apply to vector or matrix values.");
    }
    const vtkTypeUInt64 nn = ii * nc;
    for (int vv = 0; vv < nc / 3; ++vv)
    {
      vtkTypeUInt64 mm = nn + 3 * vv;
      vec = {{rr[mm], rr[mm + 1], rr[mm + 2]}};
      // TODO: Is this J^T * vec? or J*vec? Depends on whether Jacobian is row-major or column-major.
      rr[mm    ] = det * (this->Jacobian[0] * vec[0] + this->Jacobian[1] * vec[1] + this->Jacobian[2] * vec[2]);
      rr[mm + 1] = det * (this->Jacobian[3] * vec[0] + this->Jacobian[4] * vec[1] + this->Jacobian[5] * vec[2]);
      rr[mm + 2] = det * (this->Jacobian[6] * vec[0] + this->Jacobian[7] * vec[1] + this->Jacobian[8] * vec[2]);
    }
  }

  void operator() (
    vtkDataArray* cellIds, vtkDataArray* rst, vtkDoubleArray* result,
    vtkTypeUInt64 begin, vtkTypeUInt64 end) const
  {
    vtkTypeUInt64 currId;
    if (DOFSharing == SharedDOF && SourceType == Sides)
    {
      for (vtkTypeUInt64 ii = begin; ii != end; ++ii)
      {
        cellIds->GetUnsignedTuple(ii, &currId);
        this->SideConnectivity->GetUnsignedTuple(currId - this->Offset, this->SideTuple.data());
        currId = this->SideTuple[0];
        if (this->LastCellId != currId)
        {
          this->CellConnectivity->GetUnsignedTuple(currId, this->ConnTuple.data());
          std::size_t nc = this->ConnTuple.size();
          int nv = this->CellValues->GetNumberOfComponents();
          for (std::size_t jj = 0; jj < nc; ++jj)
          {
            this->CellValues->GetTuple(this->ConnTuple[jj], this->ValueTuple.data() + nv * jj);
          }
          this->LastCellId = currId;
        }
        rst->GetTuple(ii, this->RST.data());
        this->OpEntry.Op(this->RST, this->BasisTuple);
        this->InnerProduct(ii, result);
        if (Modifier == InverseJacobian)
        {
          this->ApplyInverseJacobian(ii, result);
        }
        else if (Modifier == ScaledJacobian)
        {
          this->ApplyScaledJacobian(ii, result);
        }
      }
    }
    else if (DOFSharing == SharedDOF && SourceType == Cells)
    {
      for (vtkTypeUInt64 ii = begin; ii != end; ++ii)
      {
        cellIds->GetUnsignedTuple(ii, &currId);
        if (this->LastCellId != currId)
        {
          // NB: We could ask for currId - this->Offset here, but perhaps we should
          //     assume this->Offset will always be 0 for the CellSpec?
          this->CellConnectivity->GetUnsignedTuple(currId, this->ConnTuple.data());
          std::size_t nc = this->ConnTuple.size();
          int nv = this->CellValues->GetNumberOfComponents();
          for (std::size_t jj = 0; jj < nc; ++jj)
          {
            this->CellValues->GetTuple(this->ConnTuple[jj], this->ValueTuple.data() + nv * jj);
          }
          this->LastCellId = currId;
        }
        rst->GetTuple(ii, this->RST.data());
        this->OpEntry.Op(this->RST, this->BasisTuple);
        this->InnerProduct(ii, result);
        if (Modifier == InverseJacobian)
        {
          this->ApplyInverseJacobian(ii, result);
        }
        else if (Modifier == ScaledJacobian)
        {
          this->ApplyScaledJacobian(ii, result);
        }
      }
    }
    else if (DOFSharing == Discontinuous && SourceType == Sides)
    {
      for (vtkTypeUInt64 ii = begin; ii != end; ++ii)
      {
        cellIds->GetUnsignedTuple(ii, &currId);
        this->SideConnectivity->GetUnsignedTuple(currId - this->Offset, this->SideTuple.data());
        currId = this->SideTuple[0];
        if (this->LastCellId != currId)
        {
          this->CellValues->GetTuple(currId, this->ValueTuple.data());
          this->LastCellId = currId;
        }
        rst->GetTuple(ii, this->RST.data());
        this->OpEntry.Op(this->RST, this->BasisTuple);
        this->InnerProduct(ii, result);
        if (Modifier == InverseJacobian)
        {
          this->ApplyInverseJacobian(ii, result);
        }
        else if (Modifier == ScaledJacobian)
        {
          this->ApplyScaledJacobian(ii, result);
        }
      }
    }
    else // DOFSharing == Discontinuous && SourceType == Cells
    {
      for (vtkTypeUInt64 ii = begin; ii != end; ++ii)
      {
        cellIds->GetUnsignedTuple(ii, &currId);
        // NB: We could subtract this->Offset from currId, but assume for
        //     now that CellSpec always has an offset of 0.
        if (this->LastCellId != currId)
        {
          this->CellValues->GetTuple(currId, this->ValueTuple.data());
          this->LastCellId = currId;
        }
        rst->GetTuple(ii, this->RST.data());
        this->OpEntry.Op(this->RST, this->BasisTuple);
        this->InnerProduct(ii, result);
        if (Modifier == InverseJacobian)
        {
          this->ApplyInverseJacobian(ii, result);
        }
        else if (Modifier == ScaledJacobian)
        {
          this->ApplyScaledJacobian(ii, result);
        }
      }
    }
  }
};

template<
  SharingType DOFSharing,
  SideType SourceType,
  ShapeModifier Modifier,
  SharingType ShapeSharing = Discontinuous>
void prepEntry(
  vtkDGOperation::EvaluatorEntry& entry,
  vtkDGOperatorEntry op,
  vtkDataArray* conn,
  vtkDataArray* values,
  vtkDataArray* sides,
  vtkTypeUInt64 offset,
  vtkDGOperatorEntry shapeGradient = vtkDGOperatorEntry(),
  vtkDataArray* shapeConnectivity = nullptr,
  vtkDataArray* shapeValues = nullptr
)
{
  entry.State = std::unique_ptr<vtkDGOperation::EvaluationState>(
    new OpEval<DOFSharing, SourceType, Modifier, ShapeSharing>(
      op, conn, values, sides, offset, shapeGradient, shapeConnectivity, shapeValues));
  entry.Function = [&entry](vtkDataArray* cellIds, vtkDataArray* rst, vtkDoubleArray* result,
      vtkTypeUInt64 begin, vtkTypeUInt64 end)
  {
    auto* eval = reinterpret_cast<OpEval<DOFSharing, SourceType, Modifier, ShapeSharing>*>(
      entry.State.get());
    return (*eval)(cellIds, rst, result, begin, end);
  };
}

template<
  SharingType DOFSharing,
  SideType SourceType,
  ShapeModifier Modifier,
  SharingType ShapeSharing>
void OpEval<DOFSharing, SourceType, Modifier, ShapeSharing>::CloneInto(vtkDGOperation::EvaluatorEntry& entry) const
{
  prepEntry<DOFSharing, SourceType, Modifier, ShapeSharing>(
    entry, this->OpEntry, this->CellConnectivity, this->CellValues,
    this->SideConnectivity, this->Offset,
    this->ShapeGradientEntry, this->ShapeConnectivity, this->ShapeValues);
}

vtkDGOperation::vtkDGOperation()
{
}

vtkDGOperation::vtkDGOperation(const vtkDGOperation& other)
{
  this->NumberOfResultComponents = other.NumberOfResultComponents;
  this->Evaluators.clear();
  for (const auto& otherEval : other.Evaluators)
  {
    this->Evaluators[otherEval.first] = otherEval.second;
  }
}

vtkDGOperation::vtkDGOperation(vtkDGCell* cellType, vtkCellAttribute* cellAttribute, vtkStringToken operationName)
{
  if (!this->Prepare(cellType, cellAttribute, operationName))
  {
    this->Evaluators.clear();
  }
}

void vtkDGOperation::PrintSelf(std::ostream& os, vtkIndent indent)
{
  os << indent << "Evaluators: " << this->Evaluators.size() << "\n";
  vtkIndent i2 = indent.GetNextIndent();
  for (const auto& entry : this->Evaluators)
  {
    os << i2 << "[" << entry.first.Begin << ", " << entry.first.End
      << "[  " << (entry.second.Function ? "non-null" : "null") << "\n";
  }
}

bool vtkDGOperation::Prepare(
  vtkDGCell* cellType, vtkCellAttribute* cellAttribute, vtkStringToken operationName,
  bool includeShape)
{
  this->NumberOfResultComponents = 0;
  if (!cellType || !cellAttribute || !operationName.IsValid())
  {
    return false;
  }
  auto* grid = cellType->GetCellGrid();
  if (!grid || !grid->GetShapeAttribute())
  {
    return false;
  }
  auto cellTypeInfo = cellAttribute->GetCellTypeInfo(cellType->GetClassName());
  auto opEntry = cellType->GetOperatorEntry(operationName, cellTypeInfo);
  if (!opEntry)
  {
    return false;
  }

  this->NumberOfResultComponents = cellAttribute->GetNumberOfComponents();
  this->AddSource(grid, cellType, ~0, cellAttribute, cellTypeInfo, opEntry, includeShape);
  std::size_t numSideSpecs = cellType->GetSideSpecs().size();
  for (std::size_t sideSpecIdx = 0; sideSpecIdx < numSideSpecs; ++sideSpecIdx)
  {
    this->AddSource(grid, cellType, sideSpecIdx, cellAttribute, cellTypeInfo, opEntry, includeShape);
  }
  return true;
}

bool vtkDGOperation::Evaluate(vtkDataArray* cellIds, vtkDataArray* rst, vtkDoubleArray* result)
{
  vtkTypeUInt64 nn = static_cast<vtkTypeUInt64>(cellIds->GetNumberOfTuples());
  if (nn != rst->GetNumberOfTuples())
  {
    vtkGenericWarningMacro("cellIds and rst arrays must have matching sizes.");
    return false;
  }
  // TODO: Should we do this? Or assume it is set up for us? (It could be that
  //       the caller does not want the array resized.)
  assert(result->GetNumberOfComponents() == this->NumberOfResultComponents);
  // result->SetNumberOfTuples(nn);

  bool ok = true;
  CellRangeEvaluator currEval;
  RangeKey key;
  std::map<RangeKey, EvaluatorEntry>::const_iterator eit;
  for (vtkTypeUInt64 ii = 0; ii < nn; /* do nothing */)
  {
    cellIds->GetUnsignedTuple(ii, &key.Begin);
    eit = FindEvaluator(key, this->Evaluators);
    if (eit == this->Evaluators.end() || !eit->first.Contains(key.Begin))
    {
      vtkGenericWarningMacro(
        "Invalid cell ID " << key.Begin << " at index " << ii << ". Skipping.");
      ok = false;
      // Advance to the next cell ID.
      ++ii;
      continue;
    }
    currEval = eit->second.Function;
    // Now see how many sequential entries in cellIds we can process:
    vtkTypeUInt64 jj;
    for (jj = ii + 1; jj < nn; ++jj)
    {
      cellIds->GetUnsignedTuple(jj, &key.End);
      if (!eit->first.Contains(key.End))
      {
        break;
      }
    }
    // Invoke the evaluator:
    currEval(cellIds, rst, result, ii, jj);
    // Advance to the next range. (jj > ii, so this will never stall.)
    ii = jj;
  }
  return ok;
}

vtkDGOperation::CellRangeEvaluator vtkDGOperation::GetEvaluatorForSideSpec(vtkDGCell* cell, int sideSpecId)
{
  if (!cell || sideSpecId < -1 || sideSpecId >= static_cast<int>(cell->GetSideSpecs().size()))
  {
    return nullptr;
  }

  const auto& spec(sideSpecId == -1 ? cell->GetCellSpec() : cell->GetSideSpecs()[sideSpecId]);
  if (spec.Blanked)
  {
    return nullptr;
  }
  auto it = this->Evaluators.find({static_cast<vtkTypeUInt64>(spec.Offset), 0});
  if (it == this->Evaluators.end())
  {
    return nullptr;
  }
  return it->second.Function;
}

void vtkDGOperation::AddSource(
  vtkCellGrid* grid,
  vtkDGCell* cellType, std::size_t sideSpecIdx,
  vtkCellAttribute* cellAtt, const vtkCellAttribute::CellTypeInfo& cellTypeInfo,
  vtkDGOperatorEntry& op, bool includeShape)
{
  (void)includeShape;
  const auto& cellSpec(cellType->GetCellSpec());
  bool isCellSpec = sideSpecIdx == ~0;
  const auto& source(isCellSpec ? cellType->GetCellSpec() : cellType->GetSideSpecs()[sideSpecIdx]);
  if (source.Blanked)
  {
    return; // Cannot evaluate blanked cells.
  }
  bool sharedDOF = cellTypeInfo.DOFSharing.IsValid();
  auto* values = cellTypeInfo.GetArrayForRoleAs<vtkDataArray>("values"_token);
  bool shapeSharing = false;
  vtkDataArray* shapeConn = nullptr;
  vtkDataArray* shapeValues = nullptr;
  vtkDGOperatorEntry shapeGradient;
  ShapeModifier shapeMod = None;
  if (includeShape)
  {
    if (cellTypeInfo.FunctionSpace == "HCURL")
    {
      shapeMod = InverseJacobian;
    }
    else if (cellTypeInfo.FunctionSpace == "HDIV")
    {
      shapeMod = ScaledJacobian;
    }
  }
  if (shapeMod != None)
  {
    auto shapeTypeInfo = grid->GetShapeAttribute()->GetCellTypeInfo(cellType->GetClassName());
    shapeSharing = shapeTypeInfo.DOFSharing.IsValid();
    shapeConn = shapeTypeInfo.GetArrayForRoleAs<vtkDataArray>("connectivity"_token);
    shapeValues = shapeTypeInfo.GetArrayForRoleAs<vtkDataArray>("values"_token);
    shapeGradient = cellType->GetOperatorEntry("BasisGradient"_token, shapeTypeInfo);
    if (!shapeGradient)
    {
      throw std::logic_error("No gradient operation for shape attribute.");
    }
  }
  EvaluatorEntry entry;
  // This is one huge ugly template-parameter dispatch.
  if (isCellSpec)
  {
    if (sharedDOF)
    {
      // Continuous field DOF.
      switch (shapeMod)
      {
      case None:
        {
          prepEntry<SharedDOF, Cells, None>(
            entry, op, cellSpec.Connectivity, values, nullptr, source.Offset);
        }
        break;
      case InverseJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<SharedDOF, Cells, InverseJacobian, SharedDOF>(
              entry, op, cellSpec.Connectivity, values, nullptr, source.Offset,
              shapeGradient, shapeConn, shapeValues);
          }
          else
          {
            prepEntry<SharedDOF, Cells, InverseJacobian, Discontinuous>(
              entry, op, cellSpec.Connectivity, values, nullptr, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      case ScaledJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<SharedDOF, Cells, ScaledJacobian, SharedDOF>(
              entry, op, cellSpec.Connectivity, values, nullptr, source.Offset,
              shapeGradient, shapeConn, shapeValues);
          }
          else
          {
            prepEntry<SharedDOF, Cells, ScaledJacobian, Discontinuous>(
              entry, op, cellSpec.Connectivity, values, nullptr, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      }
    }
    else
    {
      // Discontinuous field DOF
      switch (shapeMod)
      {
      case None:
        {
          prepEntry<Discontinuous, Cells, None>(
            entry, op, nullptr, values, nullptr, source.Offset);
        }
        break;
      case InverseJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<Discontinuous, Cells, InverseJacobian, SharedDOF>(
              entry, op, nullptr, values, nullptr, source.Offset,
              shapeGradient, cellSpec.Connectivity, shapeValues);
          }
          else
          {
            prepEntry<Discontinuous, Cells, InverseJacobian, Discontinuous>(
              entry, op, nullptr, values, nullptr, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      case ScaledJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<Discontinuous, Cells, ScaledJacobian, SharedDOF>(
              entry, op, nullptr, values, nullptr, source.Offset,
              shapeGradient, cellSpec.Connectivity, shapeValues);
          }
          else
          {
            prepEntry<Discontinuous, Cells, ScaledJacobian, Discontinuous>(
              entry, op, nullptr, values, nullptr, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      }
    }
  }
  else
  {
    // Processing sides, not cells:
    if (sharedDOF)
    {
      // Continuous field DOF.
      switch (shapeMod)
      {
      case None:
        {
          prepEntry<SharedDOF, Sides, None>(
            entry, op, cellSpec.Connectivity, values, source.Connectivity, source.Offset);
        }
        break;
      case InverseJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<SharedDOF, Sides, InverseJacobian, SharedDOF>(
              entry, op, cellSpec.Connectivity, values, source.Connectivity, source.Offset,
              shapeGradient, shapeConn, shapeValues);
          }
          else
          {
            prepEntry<SharedDOF, Sides, InverseJacobian, Discontinuous>(
              entry, op, cellSpec.Connectivity, values, source.Connectivity, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      case ScaledJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<SharedDOF, Sides, ScaledJacobian, SharedDOF>(
              entry, op, cellSpec.Connectivity, values, source.Connectivity, source.Offset,
              shapeGradient, shapeConn, shapeValues);
          }
          else
          {
            prepEntry<SharedDOF, Sides, ScaledJacobian, Discontinuous>(
              entry, op, cellSpec.Connectivity, values, source.Connectivity, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      }
    }
    else
    {
      // Discontinuous field DOF.
      switch (shapeMod)
      {
      case None:
        {
          prepEntry<Discontinuous, Sides, None>(
            entry, op, nullptr, values, source.Connectivity, source.Offset);
        }
        break;
      case InverseJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<Discontinuous, Sides, InverseJacobian, SharedDOF>(
              entry, op, nullptr, values, source.Connectivity, source.Offset,
              shapeGradient, shapeConn, shapeValues);
          }
          else
          {
            prepEntry<Discontinuous, Sides, InverseJacobian, Discontinuous>(
              entry, op, nullptr, values, source.Connectivity, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      case ScaledJacobian:
        {
          if (shapeSharing)
          {
            prepEntry<Discontinuous, Sides, ScaledJacobian, SharedDOF>(
              entry, op, nullptr, values, source.Connectivity, source.Offset,
              shapeGradient, shapeConn, shapeValues);
          }
          else
          {
            prepEntry<Discontinuous, Sides, ScaledJacobian, Discontinuous>(
              entry, op, nullptr, values, source.Connectivity, source.Offset,
              shapeGradient, nullptr, shapeValues);
          }
        }
        break;
      }
    }
  }
  RangeKey key{
    static_cast<vtkTypeUInt64>(source.Offset), 
    static_cast<vtkTypeUInt64>(source.Offset + source.Connectivity->GetNumberOfTuples())};
  this->Evaluators[key] = entry;
}

VTK_ABI_NAMESPACE_END
