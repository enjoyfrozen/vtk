/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#include "VTXDataArray.h"

namespace vtx
{
namespace types
{

bool DataArray::IsScalar() const noexcept
{
  if (this->VectorVariables.empty())
  {
    return true;
  }
  return false;
}

} // end namespace types
} // end namespace vtx
