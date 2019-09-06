/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkArraySort.h"

vtkArraySort::vtkArraySort() = default;

vtkArraySort::vtkArraySort(DimensionT i) :
  Storage(1)
{
  this->Storage[0] = i;
}

vtkArraySort::vtkArraySort(DimensionT i, DimensionT j) :
  Storage(2)
{
  this->Storage[0] = i;
  this->Storage[1] = j;
}

vtkArraySort::vtkArraySort(DimensionT i, DimensionT j, DimensionT k) :
  Storage(3)
{
  this->Storage[0] = i;
  this->Storage[1] = j;
  this->Storage[2] = k;
}

vtkArraySort::DimensionT vtkArraySort::GetDimensions() const
{
  return static_cast<vtkArraySort::DimensionT>(this->Storage.size());
}

void vtkArraySort::SetDimensions(DimensionT dimensions)
{
  this->Storage.assign(dimensions, 0);
}

vtkArraySort::DimensionT& vtkArraySort::operator[](DimensionT i)
{
  return this->Storage[i];
}

const vtkArraySort::DimensionT& vtkArraySort::operator[](DimensionT i) const
{
  return this->Storage[i];
}

bool vtkArraySort::operator==(const vtkArraySort& rhs) const
{
  return this->Storage == rhs.Storage;
}

bool vtkArraySort::operator!=(const vtkArraySort& rhs) const
{
  return !(*this == rhs);
}

ostream& operator<<(ostream& stream, const vtkArraySort& rhs)
{
  for(vtkArraySort::DimensionT i = 0; i != rhs.GetDimensions(); ++i)
  {
    if(i)
      stream << ",";
    stream << rhs[i];
  }

  return stream;
}
