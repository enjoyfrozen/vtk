/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkVariantCreate
 *
 * Performs an explicit conversion from an arbitrary type to a vtkVariant.  Provides
 * callers with a "hook" for defining conversions from user-defined types to vtkVariant.
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National Laboratories.
*/

#ifndef vtkVariantCreate_h
#define vtkVariantCreate_h

#include <typeinfo> // for warnings

template<typename T>
vtkVariant vtkVariantCreate(const T&)
{
  vtkGenericWarningMacro(
    << "Cannot convert unsupported type [" << typeid(T).name() << "] to vtkVariant.  "
    << "Create a vtkVariantCreate<> specialization to eliminate this warning."
    );

  return vtkVariant();
}

template<>
inline vtkVariant vtkVariantCreate<char>(const char& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<unsigned char>(const unsigned char& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<short>(const short& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<unsigned short>(const unsigned short& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<int>(const int& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<unsigned int>(const unsigned int& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<long>(const long& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<unsigned long>(const unsigned long& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<long long>(const long long& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<unsigned long long>(const unsigned long long& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<float>(const float& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<double>(const double& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<vtkStdString>(const vtkStdString& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<vtkUnicodeString>(const vtkUnicodeString& value)
{
  return value;
}

template<>
inline vtkVariant vtkVariantCreate<vtkVariant>(const vtkVariant& value)
{
  return value;
}

#endif

// VTK-HeaderTest-Exclude: vtkVariantCreate.h
