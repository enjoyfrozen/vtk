/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataArrayComputeScalarRange.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkAOSDataArrayTemplate.h" // For fast paths
#include "vtkArrayDispatch.h"
#include "vtkBitArray.h"
#include "vtkCharArray.h"
#include "vtkDataArray.h"
#include "vtkDataArrayPrivate.txx"
#include "vtkDataArrayRange.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkGenericDataArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkInformationInformationVectorKey.h"
#include "vtkInformationStringKey.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkSOADataArrayTemplate.h" // For fast paths
#ifdef VTK_USE_SCALED_SOA_ARRAYS
#include "vtkScaledSOADataArrayTemplate.h" // For fast paths
#endif
#include "vtkShortArray.h"
#include "vtkSignedCharArray.h"
#include "vtkTypeTraits.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkUnsignedShortArray.h"

#include <algorithm> // for min(), max()

namespace
{

// Wrap the DoCompute[Scalar|Vector]Range calls for vtkArrayDispatch:
struct ScalarRangeDispatchWrapper
{
  bool Success;
  double* Range;

  ScalarRangeDispatchWrapper(double* range)
    : Success(false)
    , Range(range)
  {
  }

  template <typename ArrayT>
  void operator()(ArrayT* array)
  {
    this->Success = vtkDataArrayPrivate::DoComputeScalarRange(
      array, this->Range, vtkDataArrayPrivate::AllValues());
  }
};

// Wrap the DoCompute[Scalar|Vector]Range calls for vtkArrayDispatch:
struct FiniteScalarRangeDispatchWrapper
{
  bool Success;
  double* Range;

  FiniteScalarRangeDispatchWrapper(double* range)
    : Success(false)
    , Range(range)
  {
  }

  template <typename ArrayT>
  void operator()(ArrayT* array)
  {
    this->Success = vtkDataArrayPrivate::DoComputeScalarRange(
      array, this->Range, vtkDataArrayPrivate::FiniteValues());
  }
};

} // end anon namespace

//------------------------------------------------------------------------------
bool vtkDataArray::ComputeScalarRange(double* ranges)
{
  ScalarRangeDispatchWrapper worker(ranges);
  if (!vtkArrayDispatch::Dispatch::Execute(this, worker))
  {
    worker(this);
  }
  return worker.Success;
}

//------------------------------------------------------------------------------
bool vtkDataArray::ComputeFiniteScalarRange(double* ranges)
{
  FiniteScalarRangeDispatchWrapper worker(ranges);
  if (!vtkArrayDispatch::Dispatch::Execute(this, worker))
  {
    worker(this);
  }
  return worker.Success;
}
