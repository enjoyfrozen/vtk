/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#ifndef vtkAtomicTypes_h
#define vtkAtomicTypes_h

#include "vtkAtomic.h"
#include "vtkType.h"

typedef vtkAtomic<vtkTypeInt32> vtkAtomicInt32;
typedef vtkAtomic<vtkTypeUInt32> vtkAtomicUInt32;
typedef vtkAtomic<vtkTypeInt64> vtkAtomicInt64;
typedef vtkAtomic<vtkTypeUInt64> vtkAtomicUInt64;
typedef vtkAtomic<vtkIdType> vtkAtomicIdType;

#endif
// VTK-HeaderTest-Exclude: vtkAtomicTypes.h
