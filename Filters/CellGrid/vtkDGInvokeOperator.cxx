// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkDGInvokeOperator.h"

#include <cassert>

VTK_ABI_NAMESPACE_BEGIN

namespace
{

template <typename IdType>
struct SimpleInputPoints
{
  struct Iter
  {
    std::size_t Location;
    SimpleInputPoints* Self;

    Iter operator++()
    {
      // Do not increment endlessly.
      if (this->Location == this->Self->Size)
      {
        return *this;
      }

      ++this->Location;
      return *this;
    }
    bool operator!=(const Iter& other) const
    {
      assert(this->Self == other.Self);
      return this->Location != other.Location;
    }
    IdType GetCellId() const { return this->Self->CellIds[this->Location]; }
    std::array<double, 3> GetParameter() const
    {
      std::array<double, 3> result{ this->Self->Parameters[3 * this->Location],
        this->Self->Parameters[3 * this->Location + 1],
        this->Self->Parameters[3 * this->Location + 2] };
      return result;
    }
  };

  SimpleInputPoints(std::size_t num, const IdType* cellIds, const double* rst)
    : Size(num)
    , CellIds(cellIds)
    , Parameters(rst)
  {
  }

  Iter Begin() { return Iter{ 0, this }; }
  Iter End() { return Iter{ this->Size, this }; }

  Iter begin() { return this->Begin(); }
  Iter end() { return this->End(); }

  std::size_t Size{ 0 };
  const IdType* CellIds{ nullptr };
  const double* Parameters{ nullptr };
};

template <typename IdType>
struct ArrayOfCellIdsLambdaRST
{
  struct Iter
  {
    std::size_t Location;
    ArrayOfCellIdsLambdaRST* Self;

    Iter operator++()
    {
      // Do not increment endlessly.
      if (this->Location == this->Self->Size)
      {
        return *this;
      }

      ++this->Location;
      return *this;
    }
    bool operator!=(const Iter& other)
    {
      assert(this->Self == other.Self);
      return this->Location != other.Location;
    }
    IdType GetCellId() const { return this->Self->CellIds[this->Location]; }
    std::array<double, 3> GetParameter() const
    {
      return this->Self->ParameterLambda(this->Location);
    }
  };

  ArrayOfCellIdsLambdaRST(std::size_t num, const IdType* cellIds, std::function<std::array<double,3>(IdType)> lambda)
    : Size(num)
    , CellIds(cellIds)
    , ParameterLambda(lambda)
  {
  }

  Iter Begin() { return Iter{ 0, this }; }
  Iter End() { return Iter{ this->Size, this }; }

  Iter begin() { return this->Begin(); }
  Iter end() { return this->End(); }

  std::size_t Size{ 0 };
  const IdType* CellIds{ nullptr };
  std::function<std::array<double, 3>(IdType)> ParameterLambda;
};

void dumpTuples(std::ostream& os, vtkIndent indent, vtkDataArray* array)
{
  if (array)
  {
    std::vector<double> tuple(array->GetNumberOfComponents());
    for (vtkIdType ii = 0; ii < array->GetNumberOfTuples(); ++ii)
    {
      os << indent;
      array->GetTuple(ii, tuple.data());
      for (const auto& xx : tuple)
      {
        os << " " << xx;
      }
      os << "\n";
    }
  }
}

template<typename T>
std::ostream& dumpContainer(std::ostream& os, const T& container)
{
  for (const auto& id : container)
  {
    os << " " << id;
  }
  return os;
}

} // anonymous namespace

void vtkDGInvokeOperator::FetchUnsharedCellDOF::PrintSelf(std::ostream& os, vtkIndent indent) const
{
  vtkIndent i2 = indent.GetNextIndent();
  os << indent << "Coefficients: " << this->Coefficients << "\n";
  dumpTuples(os, i2, this->Coefficients);
}

void vtkDGInvokeOperator::FetchSharedCellDOF::PrintSelf(std::ostream& os, vtkIndent indent) const
{
  vtkIndent i2 = indent.GetNextIndent();
  os << indent << "Coefficients: " << this->Coefficients << "\n";
  dumpTuples(os, i2, this->Coefficients);
  os << indent << "Connectivity: " << this->Connectivity << "\n";
  dumpTuples(os, i2, this->Connectivity);
  os << indent << "Stride: " << this->Stride << "\n";
  os << indent << "ConnTuple: ";
  dumpContainer(os, this->ConnTuple) << "\n";
}

void vtkDGInvokeOperator::FetchUnsharedSideDOF::PrintSelf(std::ostream& os, vtkIndent indent) const
{
  vtkIndent i2 = indent.GetNextIndent();
  os << indent << "Coefficients: " << this->Coefficients << "\n";
  dumpTuples(os, i2, this->Coefficients);
  os << indent << "Sides: " << this->Sides << "\n";
  dumpTuples(os, i2, this->Sides);
  os << indent << "SideTuple: ";
  dumpContainer(os, this->SideTuple) << "\n";
}

void vtkDGInvokeOperator::FetchSharedSideDOF::PrintSelf(std::ostream& os, vtkIndent indent) const
{
  vtkIndent i2 = indent.GetNextIndent();
  os << indent << "Coefficients: " << this->Coefficients << "\n";
  dumpTuples(os, i2, this->Coefficients);
  os << indent << "Connectivity: " << this->Connectivity << "\n";
  dumpTuples(os, i2, this->Connectivity);
  os << indent << "Stride: " << this->Stride << "\n";
  os << indent << "Sides: " << this->Sides << "\n";
  dumpTuples(os, i2, this->Sides);
  os << indent << "SideTuple: ";
  dumpContainer(os, this->SideTuple) << "\n";
  os << indent << "ConnTuple: ";
  dumpContainer(os, this->ConnTuple) << "\n";
}

void vtkDGInvokeOperator::PrintSelf(std::ostream& os, vtkIndent indent)
{
  vtkIndent i2 = indent.GetNextIndent();
  os << indent << "CoeffTuple: " << this->CoeffTuple.size() << "\n";
  for (const auto& xx : this->CoeffTuple) { os << i2 << xx << "\n"; }
  os << indent << "OperatorTuple: " << this->OperatorTuple.size() << "\n";
  for (const auto& xx : this->OperatorTuple) { os << i2 << xx << "\n"; }
  os << indent << "SharedFetcher:\n";
  this->SharedFetcher.PrintSelf(os, i2);
  os << indent << "SharedSideFetcher:\n";
  this->SharedSideFetcher.PrintSelf(os, i2);
  os << indent << "DiscontinuousFetcher:\n";
  this->DiscontinuousFetcher.PrintSelf(os, i2);
  os << indent << "DiscontinuousSideFetcher:\n";
  this->DiscontinuousSideFetcher.PrintSelf(os, i2);
}

bool vtkDGInvokeOperator::Invoke(const vtkDGOperatorEntry& op,
  const vtkCellAttribute::CellTypeInfo& info, std::size_t num, const vtkIdType* cellIds,
  const double* rst, double* result)
{
  if (!cellIds || !rst || !result)
  {
    return false;
  }
  if (num <= 0)
  {
    return true;
  }

  SimpleInputPoints<vtkIdType> in(num, cellIds, rst);
  return this->InvokeOp(op, info, in.begin(), in.end(), result);
}

bool vtkDGInvokeOperator::Invoke(const vtkDGOperatorEntry& op,
  const vtkCellAttribute::CellTypeInfo& info, std::size_t num, const vtkIdType* cellIds,
  ParameterLambda rstLambda, double* result)
{
  if (!cellIds || !rstLambda || !result)
  {
    return false;
  }
  if (num <= 0)
  {
    return true;
  }

  ArrayOfCellIdsLambdaRST<vtkIdType> in(num, cellIds, rstLambda);
  return this->InvokeOp(op, info, in.begin(), in.end(), result);
}

VTK_ABI_NAMESPACE_END
