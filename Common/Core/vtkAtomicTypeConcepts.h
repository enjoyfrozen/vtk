/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkAtomicTypeConcepts_h
#define vtkAtomicTypeConcepts_h

#include <limits>

namespace vtk
{
namespace atomic
{
namespace detail
{

template <bool> struct CompileTimeCheck;
template <> struct CompileTimeCheck<true> {};

template <typename T> struct IntegralType
{
  CompileTimeCheck<std::numeric_limits<T>::is_specialized &&
                   std::numeric_limits<T>::is_integer &&
                   (sizeof(T) == 4 || sizeof(T) == 8)> c;
};

} // detail
} // atomic
} // vtk

#endif
// VTK-HeaderTest-Exclude: vtkAtomicTypeConcepts.h
