/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkArrayCoordinates.h"

vtkArrayCoordinates::vtkArrayCoordinates() = default;

vtkArrayCoordinates::vtkArrayCoordinates(CoordinateT i) :
  Storage(1)
{
  this->Storage[0] = i;
}

vtkArrayCoordinates::vtkArrayCoordinates(CoordinateT i, CoordinateT j) :
  Storage(2)
{
  this->Storage[0] = i;
  this->Storage[1] = j;
}

vtkArrayCoordinates::vtkArrayCoordinates(CoordinateT i, CoordinateT j, CoordinateT k) :
  Storage(3)
{
  this->Storage[0] = i;
  this->Storage[1] = j;
  this->Storage[2] = k;
}

vtkArrayCoordinates::DimensionT vtkArrayCoordinates::GetDimensions() const
{
  return static_cast<vtkArrayCoordinates::DimensionT>(this->Storage.size());
}

void vtkArrayCoordinates::SetDimensions(DimensionT dimensions)
{
  this->Storage.assign(dimensions, 0);
}

vtkArrayCoordinates::CoordinateT& vtkArrayCoordinates::operator[](DimensionT i)
{
  return this->Storage[i];
}

const vtkArrayCoordinates::CoordinateT& vtkArrayCoordinates::operator[](DimensionT i) const
{
  return this->Storage[i];
}

vtkArrayCoordinates::CoordinateT vtkArrayCoordinates::GetCoordinate(DimensionT i) const
{
  return this->Storage[i];
}

void vtkArrayCoordinates::SetCoordinate(DimensionT i, const CoordinateT& coordinate)
{
  this->Storage[i] = coordinate;
}

bool vtkArrayCoordinates::operator==(const vtkArrayCoordinates& rhs) const
{
  return this->Storage == rhs.Storage;
}

bool vtkArrayCoordinates::operator!=(const vtkArrayCoordinates& rhs) const
{
  return !(*this == rhs);
}

ostream& operator<<(ostream& stream, const vtkArrayCoordinates& rhs)
{
  for(vtkArrayCoordinates::DimensionT i = 0; i != rhs.GetDimensions(); ++i)
  {
    if(i)
      stream << ",";
    stream << rhs[i];
  }

  return stream;
}

