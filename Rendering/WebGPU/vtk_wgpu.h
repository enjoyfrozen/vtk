#ifndef vtk_wgpu_h
#define vtk_wgpu_h

#ifdef __EMSCRIPTEN__
/*
 * We need an up-to-date version of Emscripten for the API support.
 */
#include "webgpu/webgpu_cpp.h"
#include <emscripten/html5_webgpu.h>
#if __EMSCRIPTEN_major__ == 1 &&                                                                   \
  (__EMSCRIPTEN_minor__ < 40 || (__EMSCRIPTEN_minor__ == 40 && __EMSCRIPTEN_tiny__ < 1))
#error "Emscripten 1.40.1 or higher required"
#endif
#elif defined(VTK_WEBGPU_USE_DAWN)
#include <webgpu/webgpu.h>
#elif defined(VTK_WEBGPU_USE_WGPU)
#include <webgpu.h>
#include <wgpu.h>
// Define the common instance release API for wgpu
#define wgpuAdapterRelease wgpuAdapterDrop
#define wgpuBindGroupRelease wgpuBindGroupDrop
#define wgpuBindGroupLayoutRelease wgpuBindGroupLayoutDrop
#define wgpuCommandEncoderRelease wgpuCommandEncoderDrop
#define wgpuDeviceRelease wgpuDeviceDrop
#define wgpuInstanceRelease wgpuInstanceDrop
#define wgpuRenderPassEncoderRelease wgpuRenderPassEncoderDrop
#define wgpuTextureRelease wgpuTextureDrop
#define wgpuTextureViewRelease wgpuTextureViewDrop
#define wgpuPipelineLayoutRelease wgpuPipelineLayoutDrop
#define wgpuRenderPipelineRelease wgpuRenderPipelineDrop
#endif

#endif // vtk_wgpu_h
