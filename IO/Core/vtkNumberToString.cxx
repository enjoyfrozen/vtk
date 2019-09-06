/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkNumberToString.h"

#include "vtk_doubleconversion.h"
#include VTK_DOUBLECONVERSION_HEADER(double-conversion.h)

#include <sstream>

namespace
{
template <typename TagT>
inline ostream& ToString(ostream& stream, const TagT& tag)
{
  char buf[256];
  const double_conversion::DoubleToStringConverter& converter =
    double_conversion::DoubleToStringConverter::EcmaScriptConverter();
  double_conversion::StringBuilder builder(buf, sizeof(buf));
  builder.Reset();
  converter.ToShortest(tag.Value, &builder);
  stream << builder.Finalize();
  return stream;
}
}

//----------------------------------------------------------------------------
ostream& operator<<(ostream& stream, const vtkNumberToString::TagDouble& tag)
{
  return ToString(stream, tag);
}

//----------------------------------------------------------------------------
ostream& operator<<(ostream& stream, const vtkNumberToString::TagFloat& tag)
{
  return ToString(stream, tag);
}
