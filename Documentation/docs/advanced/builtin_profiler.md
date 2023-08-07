# Optional profiler with Tracy

## Introduction

This page describes how to profile CPU, GPU and memory usage in VTK. Note that all of this is quite
advanced. It covers utilities that help a VTK developer:

1. Optimize performance of core VTK algorithms, sources, readers and writers.
2. Audit memory consumption of `vtkDataArray`/`vtkObject`
3. Profile and audit memory of individual frames rendered by the graphics subsystem

## Features

### Frame profiler

VTK can capture and emit frames as low resolution images asynchronously to a Tracy server.
This feature lets you associate individual images with a sequence of function calls in the timeline viewer
to better understand the performance implications or memory consumption in a specific frame.

### Live memory allocation map

This feature enables the visualization of memory allocations along with call stack in VTK.
It is currently limited to `malloc`/`free`(s) in `vtkBuffer` and `vtkObject`.


## Prerequisites

For this guide, you will need the following in addition to the pre-requisites to build VTK:

1. **Tracy**: [tracy](https://github.com/wolfpld/tracy)
  is a real time, nanosecond resolution, remote telemetry, hybrid frame and sampling profiler for games and other applications.

  Tracy has a remote client-server architecture where the client runs the application
  to be profiled and the server runs the viewer. The server reverse connects with the client to receive trace events from
  the VTK application.

  You will want to read the Tracy [documentation](https://github.com/wolfpld/tracy/releases/latest/download/tracy.pdf)
  thoroughly to build and use Tracy server on your platform.

## Build TracyClient

Here is a summary of how to build the `TracyClient` library. It is very simple and requires no external dependency.

1. Clone the repository

    ```sh
    git clone https://github.com/wolfpld/tracy.git
    git checkout v0.10
    ```

2. Create a build directory and run CMake, build and install

    ```sh
    mkdir build
    cd build
    cmake -S .. -B . -DTRACY_ENABLE=ON -DTRACY_STATIC=OFF -DTRACY_ON_DEMAND=ON -DCMAKE_INSTALL_PREFIX=/where/to/install/tracy
    make
    make install
    ```

That's it for TracyClient!

### Teardown of options to configure command
- `TRACY_ENABLE=ON` is quite obvious. It enables Tracy macros.
- `TRACY_STATIC=OFF` enables shared object files in order to prevent multiple tracy objects in different VTK modules
- `TRACY_ON_DEMAND=ON` This flag enables profiling only when there's an established connection with a Tracy server. Refer to the section "On-Demand profiling" in [Tracy.pdf](https://github.com/wolfpld/tracy/releases/latest/download/tracy.pdf) for the caveats of this flag.


## Build Tracy server
The server on the other hand is quite complex to build.
1. For windows, you can download the server executable from the [releases page](https://github.com/wolfpld/tracy/releases)
2. For Linux and macOS, you will need to build the `next` branch of [capstone](https://github.com/capstone-engine/capstone). Then build the profiler
from `tracy/profiler/build/unix` directory. Refer to "Building the server" in [Tracy.pdf](https://github.com/wolfpld/tracy/releases/latest/download/tracy.pdf).

## Build VTK with profiler

VTK integrates Tracy through the use of macros in `vtkProfiler.h` enabled with `VTK_ENABLE_PROFILER=ON` during the CMake configure step. By default, profiler is disabled and all `vtkProfileXXX` macros are no-ops. Do not forget to point `Tracy_DIR` to the installation path of Tracy.

## Usage

You can now annotate functions with macros. A zone contains a block of statements that were executed and profiled.

### Macros

1. `vtkProfileScoped`: starts an automatically named scoped zone.
2. `vtkProfileScopedN(name)`: nested zones, it lets you distinguish zones with custom names.
3. `vtkProfileScopedTag(tag)`: tag a scoped zone.
4. `vtkProfileScopedLog(text, size)`: append a custom message to the scoped zone.
5. `vtkProfileScopedValue(text, value)`: send a value for plotting a custom value to the scoped zone. Ex: count draw calls and plot over time: `vtkProfileScopedValue("DrawCallCount", ++drawCallCount);`
6. `vtkProfileAlloc(p, size)`: track an allocation. By default, retains upto 12 methods in the call stack
7. `vtkProfileFree(p)`: Must be used when a tracked allocation was freed! By default, retains upto 12 methods in the call stack
8. `vtkProfileAllocWithStackDepth(p, size, depth)`:  track an allocation retaining `depth` number of methods in the call stack upto this *allocation*.
9. `vtkProfileFreeWithStackDepth(p, depth)`:  track an allocation retaining `depth` number of methods in the call stack upto this *free*.

### Examples

See `vtkAlgorithm.cxx`, `vtkOpenGLPolyDataMapper.cxx` for usage of these macros.

### View realtime profile

These macros emit trace events to a Tracy Server running on the same machine or another machine (could be a different operating system).
Follow these steps to analyze VTK performance in realtime.

1. Start a tracy server on a machine, preferably with admin priviledges to view CPU context switching. The server UI will list discovered clients.
2. Connect to one of the listed machines which run your VTK based application.
