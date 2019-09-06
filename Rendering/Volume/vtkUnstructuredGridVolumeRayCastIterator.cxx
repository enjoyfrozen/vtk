/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// -*- c++ -*-

#include "vtkUnstructuredGridVolumeRayCastIterator.h"


//----------------------------------------------------------------------------

vtkUnstructuredGridVolumeRayCastIterator::vtkUnstructuredGridVolumeRayCastIterator()
{
  this->Bounds[0] = 0.0;
  this->Bounds[1] = 1.0;

  this->MaxNumberOfIntersections = 32;
}

vtkUnstructuredGridVolumeRayCastIterator::~vtkUnstructuredGridVolumeRayCastIterator() = default;

void vtkUnstructuredGridVolumeRayCastIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Bounds: [" << this->Bounds[0] << ", " << this->Bounds[1]
     << "]" << endl;
  os << indent << "MaxNumberOfIntersections: "
     << this->MaxNumberOfIntersections << endl;
}
