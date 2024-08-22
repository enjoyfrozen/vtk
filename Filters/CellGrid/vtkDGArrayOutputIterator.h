// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class   vtkDGArrayOutputIterator
 * @brief   Store values interpolated from DG cells into a vtkDoubleArray instance.
 */
#ifndef vtkDGArrayOutputIterator_h
#define vtkDGArrayOutputIterator_h

#include "vtkVector.h" // For API.
#include "vtkFiltersCellGridModule.h" // For export macro.

#include <cassert>

VTK_ABI_NAMESPACE_BEGIN

class vtkDoubleArray;

class VTKFILTERSCELLGRID_EXPORT vtkDGArrayOutputIterator
{
public:
  vtkDGArrayOutputIterator(vtkDoubleArray* result);
  vtkDGArrayOutputIterator(const vtkDGArrayOutputIterator& other);
  ~vtkDGArrayOutputIterator();

  vtkDGArrayOutputIterator& operator=(const vtkDGArrayOutputIterator& other);

  /// Expose a tuple in a vtkDoubleArray as an object with a size() method
  /// to satisfy requirements of the output iterator API.
  struct Tuple
  {
    Tuple() = default;
    Tuple(double* data, int size)
      : Data(data)
      , Size(size)
    {}
    Tuple(const Tuple&) = default;
    Tuple& operator=(const Tuple&) = default;

    double& operator[](int ii) { assert(ii < this->Size); return this->Data[ii]; }
    const double* data() const { return this->Data; }
    double* data() { return this->Data; }
    int size() const { return this->Size; }

    /// If a tuple is "null", make it "falsy"; otherwise it is "truthy."
    operator bool() const { return !!this->Data && this->Size > 0; }

    double* Data{ nullptr };
    int Size{ 0 };
  };

  Tuple operator[](vtkTypeUInt64 tupleId);
  vtkTypeUInt64 GetKey() const { return this->Key; }
  Tuple GetTuple();
  void Restart();
  bool IsAtEnd() const;

  std::size_t size() const;

  vtkTypeUInt64 operator++();
  vtkTypeUInt64 operator++(int);

  vtkDGArrayOutputIterator& operator+=(vtkTypeUInt64 count);

protected:
  vtkTypeUInt64 Key{ 0 };
  vtkDoubleArray* Result{ nullptr };
};

VTK_ABI_NAMESPACE_END

#endif // vtkDGArrayOutputIterator_h
