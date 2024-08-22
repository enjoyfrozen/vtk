// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkDGArrayOutputIterator.h"

#include "vtkDoubleArray.h"

VTK_ABI_NAMESPACE_BEGIN

vtkDGArrayOutputIterator::vtkDGArrayOutputIterator(vtkDoubleArray* result)
  : Result(result)
{
  if (result)
  {
    this->Result->Register(nullptr);
  }
}

vtkDGArrayOutputIterator::vtkDGArrayOutputIterator(const vtkDGArrayOutputIterator& other)
{
  if (&other == this)
  {
    return;
  }

  if (this->Result)
  {
    this->Result->Delete();
  }

  this->Result = other.Result;
  this->Key = other.Key;

  if (this->Result)
  {
    this->Result->Register(nullptr);
  }
}

vtkDGArrayOutputIterator::~vtkDGArrayOutputIterator()
{
  if (this->Result)
  {
    this->Result->Delete();
    this->Result = nullptr;
  }
}

vtkDGArrayOutputIterator& vtkDGArrayOutputIterator::operator=(const vtkDGArrayOutputIterator& other)
{
  if (&other == this)
  {
    return *this;
  }

  if (this->Result)
  {
    this->Result->Delete();
  }

  this->Result = other.Result;
  this->Key = other.Key;

  if (this->Result)
  {
    this->Result->Register(nullptr);
  }
  return *this;
}

vtkDGArrayOutputIterator::Tuple vtkDGArrayOutputIterator::operator[](vtkTypeUInt64 tupleId)
{
  if (tupleId >= this->Result->GetNumberOfTuples())
  {
    return Tuple();
  }
  int sz = this->Result->GetNumberOfComponents();
  return Tuple(this->Result->GetPointer(0) + tupleId * sz, sz);
}

vtkDGArrayOutputIterator::Tuple vtkDGArrayOutputIterator::GetTuple()
{
  if (this->Key >= this->Result->GetNumberOfTuples())
  {
    return Tuple();
  }
  int sz = this->Result->GetNumberOfComponents();
  return Tuple(this->Result->GetPointer(0) + this->Key * sz, sz);
}

void vtkDGArrayOutputIterator::Restart()
{
  this->Key = 0;
}

bool vtkDGArrayOutputIterator::IsAtEnd() const
{
  return this->Key >= static_cast<vtkTypeUInt64>(this->Result->GetNumberOfValues());
}

std::size_t vtkDGArrayOutputIterator::size() const
{
  return static_cast<std::size_t>(this->Result->GetNumberOfValues());
}

vtkTypeUInt64 vtkDGArrayOutputIterator::operator++()
{
  auto nn = static_cast<vtkTypeUInt64>(this->Result->GetNumberOfValues());
  if (this->Key < nn)
  {
    ++this->Key;
  }
  return this->Key;
}

vtkTypeUInt64 vtkDGArrayOutputIterator::operator++(int)
{
  auto nn = static_cast<vtkTypeUInt64>(this->Result->GetNumberOfValues());
  auto vv = this->Key;
  if (this->Key < nn)
  {
    ++this->Key;
  }
  return vv;
}

vtkDGArrayOutputIterator& vtkDGArrayOutputIterator::operator+=(vtkTypeUInt64 count)
{
  auto nn = static_cast<vtkTypeUInt64>(this->Result->GetNumberOfValues());
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
