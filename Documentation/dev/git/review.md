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

[](#coding-guidelines-modules)
### Modules

New VTK modules should consider the following questions:

- Does it belong to a "group" of modules?
- Does it belong to a "kit"?

The groups available in VTK are:

- `StandAlone`: Modules in this group are added to VTK's default build
  configuration. Generally, they should not require any external dependencies
  to build.
- `Rendering`: Modules which support rendering. These are also enabled by
  default and should not depend on external packages.
- `Imaging`: Modules handling images and filters to manipulate them. Includes
  modules to support interaction with and rendering of images as well.
- `MPI`: Modules using [MPI (Message Passing Interface)][mpi]. These are
  enabled when `VTK_USE_MPI` is enabled.
- `Qt`: Modules requiring [Qt][qt].
- `Web`: Modules supporting VTK's support for web technologies.

[mpi]: https://en.wikipedia.org/wiki/Message_Passing_Interface
[qt]: https://www.qt.io

Modules do not need to belong to any group as groups are ways of selecting a
collection of modules for enabling or disabling as a batch.

The kits available in VTK are:

- `VTK::Common`: "Core" VTK modules.
- `VTK::Filters`: Modules containing common filters without "exotic"
  dependencies.
- `VTK::Imaging`: Image manipulation modules.
- `VTK::Interaction`: Support for interacting with VTK render windows.
- `VTK::IO`: File reading and writing modules.
- `VTK::Parallel`: MPI-using modules from across the codebase.
- `VTK::Rendering`: Core rendering modules.
- `VTK::OpenGL`: OpenGL rendering modules.

Note that being part of a kit constrains the allowed dependency graph of the
module. As an example, if kit A depends on kit B, no module in kit B may depend
on any module in kit A.

[](#coding-guidelines-modules-external-deps)
#### Adding External Dependencies

New dependencies in VTK modules must use `vtk_module_find_package` to find
dependencies. This call ensures that:

- the dependency not being found is an error; and
- the dependency is exported to the build configuration properly.

A normal `find_package` may be used only if:

- it is under a `Testing` directory; or
- it is a header-only dependency that is not exposed in the public headers of
  the module (in this case pass the `PRIVATE` argument).

Some dependencies need help beyond the normal variables used by CMake in the
course of `find_package` logic. In these cases,
`CMake/vtkInstallCMakePackageHelpers.cmake` should be updated with the relevant
variables.

If possible, CI should be updated to include these dependencies (or disable the
module if it cannot be provided as CI tries to enable all modules).

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

[](#documentation)
## Documentation

Documentation in VTK

- *API documentation*: Documents available APIs, parameters, return values,
  classes, and so on. These end up rendered by [Doxygen][doxygen] and should
  use [the syntax available][doxygen-syntax] for it. Additionally, methods
  within classes may be "grouped" by using special `//@{` and `//@}` comments
  around related methods. Grouping makes Doxygen render the methods next to
  each other rather than separately (e.g., for getter and setter pairs). Some
  points to consider that a user might be interested in for APIs (though by no
  means an exhaustive list):
  - Assumptions made (e.g., non-`nullptr` pointer values)
  - Ownership of returned pointers (i.e., does the caller receive a new
    instance or is it held onto by the callee?)
  - Ownership of received pointers (i.e., does the callee take responsibility
    over the object or is it merely used as a reference?)
  - Thread safety
  - Invariants that might need to be considered
- *Usage guidelines*: Some APIs are complicated and interact with others that
  are difficult to fit onto specific methods or other places. Generally,
  documentation for classes end up containing this information.
- *Examples*: VTK contains some examples of API usage in the top-level
  `Examples` directory. They are intended to be tested as part of VTK's test
  suite, so adding them to the relevant CMake directories is recommended.
- *Release documentation*: See the
  [section on release notes](#software-process-release-notes) for further
  information.

[doxygen]: https://www.doxygen.nl/index.html
[doxygen-syntax]: https://www.star.bnl.gov/public/comp/sofi/doxygen/

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

[](#software-process)
## Software Process

There are multiple facets to VTK's software process that are used to help
manage the project as a whole. While these may feel like "extra work", they are
useful in scaling up VTK's workflow to handle the size of the codebase.
Essentially, the idea is to help streamline the work that is generated around
the process of adding new code and features to VTK itself.

[](#software-process-release-notes)
### Release Notes

VTK stores its release notes for the next release in the
`Documentation/release/dev` directory. Each file should cover one "thing"
(usually associated with a topic branch) that has changed since the last
release. Note that changes that are not mentioned in a release note file are
unlikely to be mentioned when the next release is made.

Release notes should be made for any change to VTK with user-visible effects.
This includes things such as:

- adding an API (class, method, etc.)
- changing an API (with instructions on how to adapt)
- deprecations (again, with instructions on how to adapt)
- performance improvements
- supported platform changes

Things which do *not* require release notes are those which are internal to
VTK. For example:

- fixing typos
- adding comments to code (including API documentation)
- CI configuration changes

There is a template `0-sample-topic.md` file with instructions on how to write
a release note.

[](#software-process-feedback)
### Feedback

When leaving feedback (or responding to feedback) on a merge request, it can be
very useful to batch comments during a review using the GitLab feature that
adds a comment to a review rather than posting it immediately. This can reduce
the number of emails that get sent out for reviews and keep the notification
stream into a more manageable volume.

If there are multiple instances of a fix in a merge request, it can be better
to just mention that the author should search for other instances than making a
comment on every typo, missing brace, or missing `this->` usage.

As an author replying to feedback, it is usually better to just mark a
conversation as resolved for simple fixes rather than to reply "Done" or
similar. GitLab already makes it relatively easy to see what has changed since
the last review to verify that updates are suitable.

[](#software-process-commit-structure)
### Commit Structure

When making commits, it is helpful for reviewers to be able to read the commit
messages to determine *why* a change is being made. Sometimes a simple commit
message is fine. This is usually only applicable when the diff itself is
self-evident such as `vtkFoobar: fix a typo` or `vtkFoobar: fix indentation`.
However, it is useful to communicate why a change is being made if there is
more to it.

The extra context in the commit message helps reviewers know what they'll be
looking at when they look at the change itself. Additionally, it helps to get
everyone on the same page about what the commit is trying to accomplish. If
every reviewer needs to determine what the commit is doing from the diff, their
conclusions to that end might not agree and this can result in people talking
past each other.

As a corollary, it is best if each commit does a single thing. This is so that
the commit message doesn't need to have a section to address the "and also
fixing this other thing while we're at it" change that is coming along for the
ride. Commits are not a limited resource in Git; a new one is perfectly fine.

The benefits of a useful set of commits are not just for those that trawl
through the history. Some scenarios where good commit messages can save time:

- Reviewers can already see *what* is being done in the diff itself, but a good
  commit message can explain *why* the diff is needed and how it solves the
  problem that it claims to solve. Without this information, a reviewer may be
  left wondering why this is necessary. This is especially appreciative if the
  reviewer ultimately ends up maintaining this code in the future.
- The extra context is very useful if the commit ends up being the end result
  of a bisection looking for a change in behavior. Without the additional
  context, the diff would need to stand on its own to explain why it changed
  something. When the developer doing the bisection comes back to you, the
  author, it can also be useful to help remember why the change was made in the
  first place.
- Summarizing changes over a range of commits is easier if the commit messages
  describe what is going on. This happens for VTK when ParaView updates its
  submodule commit pointer.
- Backporting fixes becomes easier if each commit is focused. Instead of having
  to deal with some incidental change or other changes that cause a conflict of
  some kind, the commit can be more easily applied to other branches.

Generally, fixing a branch once it has been pushed will require familiarity
with `git rebase`. There are numerous resources online for learning how to
leverage it. The [`git-rebase.io`][git-rebase.io] site is a one such guide.

[git-rebase.io]: https://git-rebase.io/

[](#software-process-commit-structure-examples)
#### Examples

Poor commit message usually raise more questions than they answer. Here are a
few examples:

1. `Fix bug`
  - What bug was being fixed?
  - How does the change fix it?
  - Why is this a *good* fix for the bug?
  - Where in the codebase was this bug?
2. `Address review comments`
  - What comments?
  - Whose review?
  - Where were these comments made?
  - Generally, the changes are best folded back into the commit which
    introduced the change that is being fixed.
3. `Fix clang-tidy warning`
  - Which lint?
  - The warning is in VTK, not `clang-tidy`, so the "blame" is misplaced.
4. `Fix dashboard failure`
  - Which dashboard?
  - What was the problem being fixed?
5. `Add new test baseline`
  - Which test?
  - Why is this baseline needed?
6. Stock `revert` commit messages.
  - Why was it reverted?
  - Are there followup steps that will be taken to restore the behavior
    properly?

[](#software-process-mr-metadata)
### Merge Request Metadata

GitLab provides fields for managing merge requests. VTK uses the following:

- **Assignee**: the user that should take the next action. If waiting on the
  submitter, assign the merge request to them.
- **Reviewer**: the user that is in charge of the review for this merge request
  as a whole. May be delegated for specific sections.
- **Milestone**: the targeted release for the merge request. Generally only
  used during the release cycle to mark merge requests as intended for the
  `release` branch.
- **Labels**: Categorization mechanisms for merge requests.

[](#software-process-mr-metadata-issues)
Issues may be closed or referenced in merge request descriptions. This may be
done by using a line in the form of `Fixes #xxx`.

Please refrain from *closing* issues in other projects using this mechanism as
if the issue is ever reopened, the reference can cause GitLab to close it at
another time when the commit is pushed to a fork of VTK at another time.
Instead use `See: ` to reference issues external to `vtk/vtk`.

[](#software-process-mr-metadata-robot)
Additionally, the robot which performs the checking and merging of merge
requests looks for metadata at the *end* of the description of the merge
request (one per line). Of note are the following controls:

- `Topic-rename`: GitLab does not support renaming the branch without opening
  a new merge request. The robot used for merging can take the name from this
  field instead if a better name is warranted.
- `Backport`: When merging, the robot can merge into multiple branches rather
  than just the target branch. This is used to land fixes for the `release`
  branch into `master` at the same time.

If multiple trailers are needed, blank lines may be placed between them or `  `
(two spaces) at the end of each may be used to improve the rendering in GitLab.
