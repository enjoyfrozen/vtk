/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkArrayRange.h"

#include <algorithm> // for std::max()

vtkArrayRange::vtkArrayRange() :
  Begin(0),
  End(0)
{
}

vtkArrayRange::vtkArrayRange(CoordinateT begin, CoordinateT end) :
  Begin(begin), End(std::max(begin, end))
{
}

vtkArrayRange::CoordinateT vtkArrayRange::GetBegin() const
{
  return this->Begin;
}

vtkArrayRange::CoordinateT vtkArrayRange::GetEnd() const
{
  return this->End;
}

vtkArrayRange::CoordinateT vtkArrayRange::GetSize() const
{
  return this->End - this->Begin;
}

bool vtkArrayRange::Contains(const vtkArrayRange& range) const
{
  return this->Begin <= range.Begin && range.End <= this->End;
}

bool vtkArrayRange::Contains(const CoordinateT coordinate) const
{
  return this->Begin <= coordinate && coordinate < this->End;
}

bool operator==(const vtkArrayRange& lhs, const vtkArrayRange& rhs)
{
  return lhs.Begin == rhs.Begin && lhs.End == rhs.End;
}

bool operator!=(const vtkArrayRange& lhs, const vtkArrayRange& rhs)
{
  return !(lhs == rhs);
}

ostream& operator<<(ostream& stream, const vtkArrayRange& rhs)
{
  stream << "[" << rhs.Begin << ", " << rhs.End << ")";
  return stream;
}

