# Optional profiler with Tracy

You can now profile optimized binaries based on VTK with the use of
Tracy.

## Tracy
Checkout the Tracy [documentation](https://github.com/wolfpld/tracy/releases/latest/download/tracy.pdf)
for usage and build instructions.

Tracy is a real time, nanosecond resolution, remote telemetry, hybrid frame and sampling profiler
for games and other applications.

## Profile VTK with Tracy
VTK has integrated Tracy through the use of macros in `vtkProfiler.h`. You will need to clone Tracy from
[wolfpld/tracy](https://github.com/wolfpld/tracy), configure, build and install with CMake.

### CMake
You can enable profiler in a VTK build by setting `VTK_ENABLE_PROFILER=ON` in the CMake configure step.
By default, profiler is disabled and all `vtkProfileXXX` macros are no-ops. Do not forget to point
`Tracy_DIR` to the installation path of Tracy.

### Usage
You can now annotate functions with macros such as `vtkProfileScoped`.
These macros emit trace events to a Tracy Server running on the same machine or
another machine (could be a different operating system). You can refer to `vtkAlgorithm.cxx`,
`vtkOpenGLPolyDataMapper.cxx` on how these annotations are used. Follow these steps to profile
VTK with Tracy:
1. Start a tracy server on your machine or any other machine (any platform).
2. After that, the server UI will list discovered clients.
3. Connect to one of the listed machines which run your VTK based application.

### Frame profiler
VTK also captures and emits frames asynchronously to the Tracy server. With this mechanism, you can associate
individual frames to a sequence of function calls in the timeline viewer. This feature is enabled out of the box
when VTK is configured with `VTK_ENABLE_PROFILER=ON`

### Live memory allocation map
This feature enables the visualization of memory allocations along with call stack in VTK,
currently limited to malloc/free in `vtkBuffer`. It basically applies to all sub-classes of `vtkAbstractArray`.
This feature is enabled out of the box when VTK is configured with `VTK_ENABLE_PROFILER=ON`
