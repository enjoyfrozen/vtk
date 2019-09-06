/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkArrayExtentsList.h"

vtkArrayExtentsList::vtkArrayExtentsList() = default;

vtkArrayExtentsList::vtkArrayExtentsList(const vtkArrayExtents& i) :
  Storage(1)
{
  this->Storage[0] = i;
}

vtkArrayExtentsList::vtkArrayExtentsList(const vtkArrayExtents& i, const vtkArrayExtents& j) :
  Storage(2)
{
  this->Storage[0] = i;
  this->Storage[1] = j;
}

vtkArrayExtentsList::vtkArrayExtentsList(const vtkArrayExtents& i, const vtkArrayExtents& j, const vtkArrayExtents& k) :
  Storage(3)
{
  this->Storage[0] = i;
  this->Storage[1] = j;
  this->Storage[2] = k;
}

vtkArrayExtentsList::vtkArrayExtentsList(const vtkArrayExtents& i, const vtkArrayExtents& j, const vtkArrayExtents& k, const vtkArrayExtents& l) :
  Storage(4)
{
  this->Storage[0] = i;
  this->Storage[1] = j;
  this->Storage[2] = k;
  this->Storage[3] = l;
}

vtkIdType vtkArrayExtentsList::GetCount() const
{
  return static_cast<vtkIdType>(this->Storage.size());
}

void vtkArrayExtentsList::SetCount(vtkIdType count)
{
  this->Storage.assign(count, vtkArrayExtents());
}

vtkArrayExtents& vtkArrayExtentsList::operator[](vtkIdType i)
{
  return this->Storage[i];
}

const vtkArrayExtents& vtkArrayExtentsList::operator[](vtkIdType i) const
{
  return this->Storage[i];
}

