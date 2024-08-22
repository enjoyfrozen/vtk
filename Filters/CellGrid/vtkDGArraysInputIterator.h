// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkDGArraysInputIterator
 * @brief   Evaluate DG cells whose indices and parameters are provided by vtkDataArray instances.
 */
#ifndef vtkDGArraysInputIterator_h
#define vtkDGArraysInputIterator_h

#include "vtkVector.h" // For API.
#include "vtkFiltersCellGridModule.h" // For export macro.

VTK_ABI_NAMESPACE_BEGIN

class vtkDataArray;

class VTKFILTERSCELLGRID_EXPORT vtkDGArraysInputIterator
{
public:
  vtkDGArraysInputIterator(vtkDataArray* cellIds, vtkDataArray* rst);
  vtkDGArraysInputIterator(const vtkDGArraysInputIterator& other);
  ~vtkDGArraysInputIterator();

  vtkDGArraysInputIterator& operator=(const vtkDGArraysInputIterator& other);

  vtkIdType GetCellId(vtkTypeUInt64 iteration);
  vtkVector3d GetParameter(vtkTypeUInt64 iteration);
  vtkTypeUInt64 GetKey() const { return this->Key; }
  void Restart();
  bool IsAtEnd() const;

  std::size_t size() const;

  vtkTypeUInt64 operator++();
  vtkTypeUInt64 operator++(int);

  vtkDGArraysInputIterator& operator+=(vtkTypeUInt64 count);

protected:
  vtkTypeUInt64 Key{ 0 };
  vtkDataArray* CellIds{ nullptr };
  vtkDataArray* RST{ nullptr };
};

VTK_ABI_NAMESPACE_END

#endif // vtkDGArraysInputIterator_h
