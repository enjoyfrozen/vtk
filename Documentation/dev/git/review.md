# VTK Review Process

This page documents how to review VTK through its [GitLab
project](https://gitlab.kitware.com/vtk/vtk/-/merge_requests). See the
[README](README.md) for more information.

[](#coding-guidelines)
## Coding Guidelines

VTK has some coding guidelines that should be followed when editing its codebase.

[](#coding-guidelines-cpp)
### C++

[](#coding-guidelines-cpp-pound-if)
- Prefer `#if VARIABLE` to `#ifdef VARIABLE` for VTK-defined symbols. This can
  better detect if the symbol is missing due to a missing `#include` or other
  mechanism rather than silently failing.
- All references to a class' methods and members must use `this->` to access
  them (note that referencing static members should use `vtkClassName::`
  instead; `clang-tidy` will point these out).

| Bad | Good |
|-----|------|
| `StaticFunctionCall(MemberVariable)` | `StaticFunctionCall(this->MemberVariable)` |
| `MethodCall(MemberVariable)` | `this->MethodCall(this->MemberVariable)` |
| `StaticMethodCall(StaticMemberVariable)` | `vtkClassName::MethodCall(vtkClassName::StaticMemberVariable)` |
| `StaticFunctionCall(MemberVariable)` | `StaticFunctionCall(this->ParentMember)` |

- All blocks must use explicit `{` and `}` around them. This includes
  single-statement blocks.

<table>
<tr>
<th>Bad</th>
<th>Good</th>
</tr>
<tr>
<td>
```c++
if (cond)
    body();
```
</td>
<td>
```c++
if (cond)
{
  body();
}
```
</td>
</tr>
</table>

- New class members should be added to the `PrintSelf` method so that they
  appear when the object is output using this debugging mechanism.

[](#coding-guidelines-cmake)
### CMake

VTK's CMake code is fairly consistent and it'd be nice to keep it that way :) .

[](#coding-guidelines-cmake-minimum)
#### Minimum Version

VTK has a minimum CMake version of 3.13 in its public APIs. Any usage of new
features must be guarded by CMake version or policy checks.

[](#coding-guidelines-cmake-variable)
#### Variable Guidelines

- Do not use cache variables for internal tracking. Prefer global properties if
  the values do not need to persist between configures.
- Document new cache variables in [the VTK build documentation](../../build.md).
- Variable naming guidelines
  - Lowercase for internal values (derived from other values or bookkeeping).
  - Uppercase for values that can be influenced by the "outside" (cache
    variables, environment variables, etc.).
- Quote *all* variable expansions except:
  - when the quote would change the meaning (such as in `if` conditionals
  - when the variable expands to a list of arguments being passed to the
    command in question

[](#coding-guidelines-cmake-property)
#### Properties

There are some CMake patterns which are improvements on more "obvious" ways of
doing things.

- Prefer `COMPILE_DEFINITIONS "CONDITIONAL=$<BOOL:${variable}>"` to setting or
  not setting the definition (see [`#if`](#pound-if) guidelines).
- Prefer `set_property(APPEND)` to `set_X_properties` to avoid clobbering an
  already set value.

<table>
<tr>
<th>Bad</th>
<th>Good</th>
</tr>
<tr>
<td>
```cmake
if (variable)
  set_property(SOURCE vtkFoo.cxx APPEND
    PROPERTY
      COMPILE_DEFINITIONS SOME_VARIABLE)
endif ()
```
</td>
<td>
```cmake
set_property(SOURCE vtkFoo.cxx APPEND
  PROPERTY
    COMPILE_DEFINITIONS "SOME_VARIABLE=$<BOOL:${variable}>")
```
</td>
</tr>
<tr>
<td>
```cmake
set_source_files_properties(vtkFoo.cxx
  PROPERTIES
    LABELS CUDA)
```
</td>
<td>
```cmake
set_property(SOURCE vtkFoo.cxx APPEND
  PROPERTY
    LABELS CUDA)
```
</td>
</tr>
</table>

[](#coding-guidelines-cmake-style)
#### Style Guidelines

- Use spaces after control flow commands such as `if`, `while`, `elseif`,
  `macro`, `function`, `return`, `break`, and `continue` (and any `end*`
  variants) and not otherwise.
- Two-space indentation.
- Keep lists of filenames sorted (`clang-format` will keep `#include`
  directives sorted automatically). This helps to avoid duplicates and
  conflicts that occur when always adding to the end.

<table>
<tr>
<th>Bad</th>
<th>Good</th>
</tr>
<tr>
<td>
```cmake
if(cond)
  blah()
endif()
```
</td>
<td>
```cmake
if (cond)
  blah()
endif ()
```
</td>
</tr>
<tr>
<td>
```cmake
if (cond)
    blah()
endif ()
```
</td>
<td>
```cmake
if (cond)
  blah()
endif ()
```
</td>
</tr>
<tr>
<td>
```cmake
set(classes
  vtkOld
  vtkNew)
```
</td>
<td>
```cmake
set(classes
  vtkNew
  vtkOld)
```
</td>
</tr>
</table>

[](#coding-guidelines-cmake-api)
#### API Guidelines

VTK-internal (i.e., not listed under `vtk_cmake_module_files` in
[`CMake/vtkInstallCMakePackage.cmake`](../../../CMake/vtkInstallCMakePackage.cmake)
APIs can relax these guidelines a bit, but it is best to follow them even then.

- APIs should be documented for Doxygen using `[==[.md` long-comment blocks
  (see [`CMake/vtkModule.cmake`](../../../CMake/vtkModule.cmake) for examples).
- Arguments must be strictly checked using `cmake_parse_arguments` and raising
  an error on unrecognized arguments.
- Use a common prefix for all function-local variable names, preferably
  starting with `_` as well in order to avoid conflicts from other scopes.
- Strongly prefer `function` to `macro` (the only use cases for `macro` should
  be something that needs to export an unknown set of variables to the caller
  or where the `macro` needs to call `return` for the caller).
- Prefer explicit named arguments to "ambient" variables that don't have
  well-defined meanings (such as `BUILD_SHARED_LIBS` or `CMAKE_GENERATOR`).
  Defaulting argument values to available "ambient" variables is fine, but
  there should be a way to override it through a named argument.
- Prefer single-value boolean arguments to present/not-present keywords. This
  allows the caller to pass `BOOL_ARG "${custom_value}"` rather than
  conditionally building up an argument list.
