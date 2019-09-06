/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

//----------------------------------------------------------------------------
// Templated definition of ToNumeric, isolated into its very own file to
// allow it to be defined before its use with most compilers, but after its
// use for (at least) Visual Studio 6.

template <typename T>
T vtkVariant::ToNumeric(bool* valid, T* vtkNotUsed(ignored)) const
{
  if (valid)
  {
    *valid = true;
  }
  if (this->IsString())
  {
    return vtkVariantStringToNumeric<T>(* this->Data.String, valid);
  }
  if (this->IsFloat())
  {
    return static_cast<T>(this->Data.Float);
  }
  if (this->IsDouble())
  {
    return static_cast<T>(this->Data.Double);
  }
  if (this->IsChar())
  {
    return static_cast<T>(this->Data.Char);
  }
  if (this->IsUnsignedChar())
  {
    return static_cast<T>(this->Data.UnsignedChar);
  }
  if (this->IsSignedChar())
  {
    return static_cast<T>(this->Data.SignedChar);
  }
  if (this->IsShort())
  {
    return static_cast<T>(this->Data.Short);
  }
  if (this->IsUnsignedShort())
  {
    return static_cast<T>(this->Data.UnsignedShort);
  }
  if (this->IsInt())
  {
    return static_cast<T>(this->Data.Int);
  }
  if (this->IsUnsignedInt())
  {
    return static_cast<T>(this->Data.UnsignedInt);
  }
  if (this->IsLong())
  {
    return static_cast<T>(this->Data.Long);
  }
  if (this->IsUnsignedLong())
  {
    return static_cast<T>(this->Data.UnsignedLong);
  }
  if (this->IsLongLong())
  {
    return static_cast<T>(this->Data.LongLong);
  }
  if (this->IsUnsignedLongLong())
  {
    return static_cast<T>(this->Data.UnsignedLongLong);
  }
  // For arrays, convert the first value to the appropriate type.
  if (this->IsArray())
  {
    if (this->Data.VTKObject->IsA("vtkDataArray"))
    {
      // Note: This are not the best conversion.
      //       We covert the first value to double, then
      //       cast it back to the appropriate numeric type.
      vtkDataArray* da = vtkDataArray::SafeDownCast(this->Data.VTKObject);
      return static_cast<T>(da->GetTuple1(0));
    }
    if (this->Data.VTKObject->IsA("vtkVariantArray"))
    {
      // Note: This are not the best conversion.
      //       We covert the first value to double, then
      //       cast it back to the appropriate numeric type.
      vtkVariantArray* va = vtkVariantArray::SafeDownCast(this->Data.VTKObject);
      return static_cast<T>(va->GetValue(0).ToDouble());
    }
    if (this->Data.VTKObject->IsA("vtkStringArray"))
    {
      vtkStringArray* sa = vtkStringArray::SafeDownCast(this->Data.VTKObject);
      return vtkVariantStringToNumeric<T>(sa->GetValue(0), valid);
    }
  }
  if (valid)
  {
    *valid = false;
  }
  return static_cast<T>(0);
}
