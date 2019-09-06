/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkAtomic.h"

namespace detail
{

vtkTypeInt64 AtomicOps<8>::AddAndFetch(vtkTypeInt64 *ref, vtkTypeInt64 val)
{
  vtkTypeInt64 result;
# pragma omp atomic capture
  {
    (*ref) += val;
    result = *ref;
  }
# pragma omp flush
  return result;
}

vtkTypeInt64 AtomicOps<8>::SubAndFetch(vtkTypeInt64 *ref, vtkTypeInt64 val)
{
  vtkTypeInt64 result;
# pragma omp atomic capture
  {
    (*ref) -= val;
    result = *ref;
  }
# pragma omp flush
  return result;
}

vtkTypeInt64 AtomicOps<8>::PreIncrement(vtkTypeInt64 *ref)
{
  vtkTypeInt64 result;
# pragma omp atomic capture
  result = ++(*ref);
# pragma omp flush
  return result;
}

vtkTypeInt64 AtomicOps<8>::PreDecrement(vtkTypeInt64 *ref)
{
  vtkTypeInt64 result;
# pragma omp atomic capture
  result = --(*ref);
# pragma omp flush
  return result;
}

vtkTypeInt64 AtomicOps<8>::PostIncrement(vtkTypeInt64 *ref)
{
  vtkTypeInt64 result;
# pragma omp atomic capture
  result = (*ref)++;
# pragma omp flush
  return result;
}

vtkTypeInt64 AtomicOps<8>::PostDecrement(vtkTypeInt64 *ref)
{
  vtkTypeInt64 result;
# pragma omp atomic capture
  result = (*ref)--;
# pragma omp flush
  return result;
}

vtkTypeInt64 AtomicOps<8>::Load(const vtkTypeInt64 *ref)
{
  vtkTypeInt64 result;
# pragma omp flush
# pragma omp atomic read
  result = *ref;
  return result;
}

void AtomicOps<8>::Store(vtkTypeInt64 *ref, vtkTypeInt64 val)
{
# pragma omp atomic write
  *ref = val;
# pragma omp flush
}


vtkTypeInt32 AtomicOps<4>::AddAndFetch(vtkTypeInt32 *ref, vtkTypeInt32 val)
{
  vtkTypeInt32 result;
# pragma omp atomic capture
  {
    (*ref) += val;
    result = *ref;
  }
# pragma omp flush
  return result;
}

vtkTypeInt32 AtomicOps<4>::SubAndFetch(vtkTypeInt32 *ref, vtkTypeInt32 val)
{
  vtkTypeInt32 result;
# pragma omp atomic capture
  {
    (*ref) -= val;
    result = *ref;
  }
# pragma omp flush
  return result;
}

vtkTypeInt32 AtomicOps<4>::PreIncrement(vtkTypeInt32 *ref)
{
  vtkTypeInt32 result;
# pragma omp atomic capture
  result = ++(*ref);
# pragma omp flush
  return result;
}

vtkTypeInt32 AtomicOps<4>::PreDecrement(vtkTypeInt32 *ref)
{
  vtkTypeInt32 result;
# pragma omp atomic capture
  result = --(*ref);
# pragma omp flush
  return result;
}

vtkTypeInt32 AtomicOps<4>::PostIncrement(vtkTypeInt32 *ref)
{
  vtkTypeInt32 result;
# pragma omp atomic capture
  result = (*ref)++;
# pragma omp flush
  return result;
}

vtkTypeInt32 AtomicOps<4>::PostDecrement(vtkTypeInt32 *ref)
{
  vtkTypeInt32 result;
# pragma omp atomic capture
  result = (*ref)--;
# pragma omp flush
  return result;
}

vtkTypeInt32 AtomicOps<4>::Load(const vtkTypeInt32 *ref)
{
  vtkTypeInt32 result;
# pragma omp flush
# pragma omp atomic read
  result = *ref;
  return result;
}

void AtomicOps<4>::Store(vtkTypeInt32 *ref, vtkTypeInt32 val)
{
# pragma omp atomic write
  *ref = val;
# pragma omp flush
}

}
