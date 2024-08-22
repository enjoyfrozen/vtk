// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkDGArraysInputIterator.h"

#include "vtkDataArray.h"

VTK_ABI_NAMESPACE_BEGIN

vtkDGArraysInputIterator::vtkDGArraysInputIterator(vtkDataArray* cellIds, vtkDataArray* rst)
  : CellIds(cellIds)
  , RST(rst)
{
  if (cellIds)
  {
    this->CellIds->Register(nullptr);
  }
  if (rst)
  {
    this->RST->Register(nullptr);
  }
}

vtkDGArraysInputIterator::vtkDGArraysInputIterator(const vtkDGArraysInputIterator& other)
{
  if (&other == this)
  {
    return;
  }

  if (this->CellIds)
  {
    this->CellIds->Delete();
  }
  if (this->RST)
  {
    this->RST->Delete();
  }

  this->CellIds = other.CellIds;
  this->RST = other.RST;
  this->Key = other.Key;

  if (this->CellIds)
  {
    this->CellIds->Register(nullptr);
  }
  if (this->RST)
  {
    this->RST->Register(nullptr);
  }
}

vtkDGArraysInputIterator::~vtkDGArraysInputIterator()
{
  if (this->CellIds)
  {
    this->CellIds->Delete();
    this->CellIds = nullptr;
  }
  if (this->RST)
  {
    this->RST->Delete();
    this->RST = nullptr;
  }
}

vtkDGArraysInputIterator& vtkDGArraysInputIterator::operator=(const vtkDGArraysInputIterator& other)
{
  if (&other == this)
  {
    return *this;
  }

  if (this->CellIds)
  {
    this->CellIds->Delete();
  }
  if (this->RST)
  {
    this->RST->Delete();
  }

  this->CellIds = other.CellIds;
  this->RST = other.RST;
  this->Key = other.Key;

  if (this->CellIds)
  {
    this->CellIds->Register(nullptr);
  }
  if (this->RST)
  {
    this->RST->Register(nullptr);
  }
  return *this;
}

vtkIdType vtkDGArraysInputIterator::GetCellId(vtkTypeUInt64 iteration)
{
  vtkTypeUInt64 cellId;
  this->CellIds->GetUnsignedTuple(iteration, &cellId);
  return static_cast<vtkIdType>(cellId);
}

vtkVector3d vtkDGArraysInputIterator::GetParameter(vtkTypeUInt64 iteration)
{
  vtkVector3d rst;
  this->RST->GetTuple(iteration, rst.GetData());
  return rst;
}

void vtkDGArraysInputIterator::Restart()
{
  this->Key = 0;
}

bool vtkDGArraysInputIterator::IsAtEnd() const
{
  return this->Key >= static_cast<vtkTypeUInt64>(this->CellIds->GetNumberOfValues());
}

std::size_t vtkDGArraysInputIterator::size() const
{
  return static_cast<std::size_t>(this->CellIds->GetNumberOfValues());
}

vtkTypeUInt64 vtkDGArraysInputIterator::operator++()
{
  auto nn = static_cast<vtkTypeUInt64>(this->CellIds->GetNumberOfValues());
  if (this->Key < nn)
  {
    ++this->Key;
  }
  return this->Key;
}

vtkTypeUInt64 vtkDGArraysInputIterator::operator++(int)
{
  auto nn = static_cast<vtkTypeUInt64>(this->CellIds->GetNumberOfValues());
  auto vv = this->Key;
  if (this->Key < nn)
  {
    ++this->Key;
  }
  return vv;
}

vtkDGArraysInputIterator& vtkDGArraysInputIterator::operator+=(vtkTypeUInt64 count)
{
  auto nn = static_cast<vtkTypeUInt64>(this->CellIds->GetNumberOfValues());
  if (this->Key + count > nn)
  {
    this->Key = nn;
  }
  else
  {
    this->Key += nn;
  }
  return *this;
}

VTK_ABI_NAMESPACE_END
