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

[](#testing)
## Testing

VTK relies on testing to ensure that its components continue to work. Tests are
added for each module under the `Testing` directory next to the source of the
module.

Tests should be added for any change which has a user-visible behavior change
(basically "everything"). This is especially true when it is a fix to an issue
that has been reported.

Ideally, existing test code should not need updated for API changes. If some
test code needs to be updated because of an API change, then there is a good
chance that VTK consumers could also have been broken. This is against VTK's
stability goals and instead, existing code should be kept working as much as
possible.

[](#testing-cpp)
### C++

C++ tests are placed under the `Testing/Cxx` directory. Each file ends up as
its own test in CTest and CDash.

The base filename should match the function name provided in the file with a
signature like `main`. The function should return `EXIT_SUCCESS` or
`EXIT_FAILURE` depending on whether the test succeeded or not.

```c++
int TestComponent(int argc, char* argv[])
{
  // …
}
```

VTK's test harness will ensure that this function is called when the test is
executed.

[](#testing-python)
### Python

Python tests are placed under the `Testing/Python` directory. Each file ends up
as its own test in CTest and CDash.

Python tests should consist of classes inheriting from
`vtkmodules.test.Testing.vtkTest` and contain methods starting with a prefix
which will be run as unit tests. The test class and its prefix are passed to
`Testing.main` as part of a list of classes to test.

```python
from vtkmodules.test import Testing

class TestComponent(Testing.vtkTest):
    def testOne(self):
        self.assertEqual(1, 1)

if __name__ == "__main__":
    Testing.main([(TestComponent, 'test')])
```

[](#testing-data)
### Data

Some tests will require input data or baseline images to be provided. By
default, these arguments are passed to tests and may be retrieved. The
`vtkTesting` class handles parsing these arguments automatically.

```c++
vtkNew<vtkTesting> testing;
testing->AddArguments(argc, argv);

// Where to find input data.
testing->GetDataRoot();
// Where to write intermediate files.
testing->GetTempDirectory();

// Test a render window against the baseline images.
int status = vtkRegressionTestImage(render_window);
// See `vtkTesting::ReturnValue` for `status` values.
```

```python
from vtkmodules.util import misc

# Where to find input data.
misc.vtkGetDataRoot()
# Where to write intermediate files.
misc.vtkGetTempDir()

# Test a render window against the baseline images.
status = misc.vtkRegressionTestImage(render_window)
if status == 2:
    # No baseline image given.
    pass
```

Please see the [data workflow](data.md) documentation for adding or replacing
data in VTK's source tree.

[](#testing-catching-errors)
### Catching Errors

Some tests need to test behaviors that trigger errors within the class being
tested. This ends up being printed to the test's output and caught by CTest as
a test failure. There is the `vtkTestErrorObserver` class which may be used to
capture errors from a `vtkObject` and stored for testing later.

```c++
vtkNew<vtkFrob> frob;
vtkNew<vtkTest::ErrorObserver> observer;
frob->AddObserver(vtkCommand::ErrorEvent, observer);
frob->TriggerError();
int status = observer->CheckErrorMessage("expected error message content");
// status == 1 → no match
// status == 0 → error string matched
```

[](#testing-skipping)
### Skipping

Sometimes a test may determine that it does not have the required environment
in order to give a useful result at runtime. This should be done for situations
that are outside of VTK's control and do not require a recompilation of VTK to
fix. This can include situations such as:

- insufficient graphics resources (memory, OpenGL capabilities, etc.)
- Python modules are not available
- a hardware device is not found
- a network resource is unavailable

In these cases, the test should exit with a "skip return code". In Python, the
`vtkmodules.test.Testing.skip()` function will do this. In C++, the
`VTK_SKIP_RETURN_CODE` value may be returned (provided by the `vtkTesting.h`
header).
