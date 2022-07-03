## Adding vtkSetEnumClampMacro for enum class support

After addition of vtkSetEnumMacro and vtkGetEnumMacro, there was no addition
of a Clamp version. A new macro was created, called vtkSetEnumClampMacro.
It uses the internal representation type of the enum (usually, integer), but
the input and output are of the enum class type. This relies on the existence
of operator< and operator> for the underlying internal type representation of
the enum.
