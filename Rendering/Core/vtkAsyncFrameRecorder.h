// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class vtkAsyncFrameRecorder
 * @brief A non-blocking frame recorder for vtkRenderWindow
 *
 * This class is an abstract interface that lets you record frames rendered by
 * a vtkRenderWindow. It can be useful in many cases:
 *  1. When profiling graphics functions (OpenGL, etc), it lets you visually associate
 *     an image with a sequence of function calls in the timeline.
 *  2. Whereas traditional frame capture with `vtkWindowToImageFilter` blocks the main (or) renderer
 *     thread. This class is tremendously useful if you do not need the frames right away, as is the
 * case when you're writing a movie or sequence of images in a background thread.
 *
 * @note Call vtkAsyncFrameRecorder::RetrieveAvailableFrames(), then call
 * vtkAsyncFrameRecorder::Capture this just before buffers are swapped.
 */

#ifndef vtkAsyncFrameRecorder_h
#define vtkAsyncFrameRecorder_h

#include "vtkObject.h"

#include "vtkCommand.h"             // for UserEvent
#include "vtkRenderingCoreModule.h" // export macro

VTK_ABI_NAMESPACE_BEGIN
class vtkWindow;
class vtkRenderWindow;

class VTKRENDERINGCORE_EXPORT vtkAsyncFrameRecorder : public vtkObject
{
public:
  vtkTypeMacro(vtkAsyncFrameRecorder, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  enum
  {
    /**
     * Event fired when a frame was captured and retrieved from the rendering subsystem.
     */
    FrameCapturedEvent = vtkCommand::UserEvent + 1
  };

  /**
   * Capture a frame from the render window.
   * @note This method is a fire and forget i.e, non-blocking.
   * @warning THIS METHOD IS NOT THREAD SAFE.
   */
  virtual void Capture(
    vtkRenderWindow* window, int srcWidth, int srcHeight, int dstWidth, int dstHeight) = 0;

  /**
   * Retrieve frames that were captured. When you invoke this method, this class fires
   * vtkAsyncFrameRecorder::FrameCapturedEvent repeatedly with the callData populated with a
   * vtkImageData until the backlog goes down to 0. Call this once before
   * vtkAsyncFrameRecorder::Capture() to ensure correct order of operation.
   * @warning THIS METHOD IS NOT THREAD SAFE.
   */
  virtual void RetrieveAvailableFrames() = 0;

  /**
   * Get number of frames in backlog.
   * @warning THIS METHOD IS NOT THREAD SAFE.
   */
  virtual std::size_t GetBacklogSize() const = 0;

  virtual void ReleaseGraphicsResources(vtkWindow* window) = 0;

protected:
  vtkAsyncFrameRecorder();
  ~vtkAsyncFrameRecorder() override;

  int LastSrcWidth = 0;
  int LastSrcHeight = 0;
  int LastDstWidth = 0;
  int LastDstHeight = 0;

private:
  vtkAsyncFrameRecorder(const vtkAsyncFrameRecorder&) = delete;
  void operator=(const vtkAsyncFrameRecorder&) = delete;
};
VTK_ABI_NAMESPACE_END

#endif // vtkAsyncFrameRecorder_h
