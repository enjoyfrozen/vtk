/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class vtkNumberToString
 * @brief Convert floating and fixed point numbers to strings
 *
 * This class uses the double-conversion library to convert floating point and
 * fixed point numbers to ASCII versions that are represented without
 * numerical precision errors.
 *
 * Typical use:
 *
 * @code{cpp}
 *  #include "vtkNumberToString.h"
 *  vtkNumberToString convert;
 *  float a = 1.0f/3.0f;
 *  std::cout << convert(a) << std::endl;
 * @endcode
 */
#ifndef vtkNumberToString_h
#define vtkNumberToString_h

#include "vtkIOCoreModule.h" // For export macro
#include "vtkTypeTraits.h"

#include <ostream>
#include <string>

class VTKIOCORE_EXPORT vtkNumberToString
{
public:
  struct TagDouble
  {
    double Value;
    TagDouble(const double& value) : Value(value) {}
  };

  struct TagFloat
  {
    float Value;
    TagFloat(const float& value) : Value(value) {}
  };

  template <typename T>
  const T& operator()(const T& val) const
  {
    return val;
  }
  const TagDouble operator()(const double& val) const { return TagDouble(val); }
  const TagFloat operator()(const float& val) const { return TagFloat(val); }
};

VTKIOCORE_EXPORT ostream& operator<<(ostream& stream, const vtkNumberToString::TagDouble& tag);
VTKIOCORE_EXPORT ostream& operator<<(ostream& stream, const vtkNumberToString::TagFloat& tag);

#endif
// VTK-HeaderTest-Exclude: vtkNumberToString.h
