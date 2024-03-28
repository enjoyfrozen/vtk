// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class vtkOpenGLAsyncFrameRecorder
 * @brief Implements frame capture and retrieval API of vtkAsyncFrameRecorder
 */

#ifndef vtkOpenGLAsyncFrameRecorder_h
#define vtkOpenGLAsyncFrameRecorder_h

#include "vtkAsyncFrameRecorder.h"

#include "vtkNew.h"                    // for ivar
#include "vtkRenderingOpenGL2Module.h" // export macro
#include "vtkSmartPointer.h"           // for arg
#include "vtk_glew.h"                  // for GLsync, GLuint

VTK_ABI_NAMESPACE_BEGIN
class vtkShaderProgram;
class vtkRenderWindow;
class vtkOpenGLVertexArrayObject;
class vtkUnsignedCharArray;
class vtkOpenGLRenderWindow;
class vtkWindow;
class vtkPixelBufferObject;
class vtkOpenGLFramebufferObject;
class vtkTextureObject;

class VTKRENDERINGOPENGL2_EXPORT vtkOpenGLAsyncFrameRecorder : public vtkAsyncFrameRecorder
{
public:
  static vtkOpenGLAsyncFrameRecorder* New();
  vtkTypeMacro(vtkOpenGLAsyncFrameRecorder, vtkAsyncFrameRecorder);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Implement super class interface.
   * @warning NONE OF THESE METHODS ARE THREAD SAFE.
   */
  void Capture(
    vtkRenderWindow* window, int srcWidth, int srcHeight, int dstWidth, int dstHeight) override;
  void RetrieveAvailableFrames() override;
  std::size_t GetBacklogSize() const override;
  void ReleaseGraphicsResources(vtkWindow* window) override;
  ///@}

protected:
  vtkOpenGLAsyncFrameRecorder();
  ~vtkOpenGLAsyncFrameRecorder() override;

  /// @brief Fence for GL_SYNC_GPU_COMMANDS_COMPLETE
  GLsync Fences[4];
  /// @brief PBO used to asynchronously read pixels from the GPU in the background
  vtkNew<vtkPixelBufferObject> PBOs[4];
  /// @brief RGBA color attachment with size = srcWidth x srcHeight
  vtkNew<vtkTextureObject> ActualSizeTextures[4];
  /// @brief Framebuffer with above color attachment of size = srcWidth x srcHeight
  vtkNew<vtkOpenGLFramebufferObject> ActualSizeFramebuffers[4];
  /// @brief RGBA color attachment with size = dstWidth x dstHeight
  vtkNew<vtkTextureObject> ScaledTextures[4];
  /// @brief Framebuffer with above color attachment of size = dstWidth x dstHeight
  vtkNew<vtkOpenGLFramebufferObject> ScaledFramebuffers[4];
  /// @brief OpenGL pipeline to scale up/down frame and read into PBO.
  vtkShaderProgram* Program = nullptr;
  vtkNew<vtkOpenGLVertexArrayObject> VAO;

  /// @brief True when all GL resources are setup.
  bool Initialized = false;
  /// @brief An index into the completion queue. Used to insert new captures.
  int FrameIndex = 0;
  /// @brief A ring buffer of frame indices at the time of capture.
  std::vector<int> FrameQueue;

  void PrepShaderProgram(vtkOpenGLRenderWindow* oglRenWin);
  void Setup(
    vtkOpenGLRenderWindow* oglRenWin, int srcWidth, int srcHeight, int dstWidth, int dstHeight);
  void TearDown(vtkOpenGLRenderWindow* oglRenWin);

private:
  vtkOpenGLAsyncFrameRecorder(const vtkOpenGLAsyncFrameRecorder&) = delete;
  void operator=(const vtkOpenGLAsyncFrameRecorder&) = delete;
};
VTK_ABI_NAMESPACE_END

#endif // vtkOpenGLAsyncFrameRecorder_h
