/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkVRRenderWindow.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkVRRenderWindow
 * @brief   VR rendering window
 *
 *
 * vtkVRRenderWindow is a abstract class to define a RenderWindow in a
 * VR context.
 *
 * VR provides HMD and controller positions in "Physical" coordinate
 * system.
 * Origin: user's eye position at the time of calibration.
 * Axis directions: x = user's right; y = user's up; z = user's back.
 * Unit: meter.
 *
 * Renderer shows actors in World coordinate system. Transformation between
 * Physical and World coordinate systems is defined by PhysicalToWorldMatrix.
 * This matrix determines the user's position and orientation in the rendered
 * scene and scaling (magnification) of rendered actors.
 *
 */

#ifndef vtkVRRenderWindow_h
#define vtkVRRenderWindow_h

#include "vtkEventData.h"    // for enums
#include "vtkNew.h"          // for vtkNew
#include "vtkOpenGLHelper.h" // used for ivars
#include "vtkOpenGLRenderWindow.h"
#include "vtkRenderingVRModule.h" // For export macro
#include "vtkSmartPointer.h"      // for vtkSmartPointer
#include "vtk_glew.h"             // used for methods

#include <vector> // ivars

class vtkCamera;
class vtkMatrix4x4;
class vtkTransform;
class vtkVRModel;

class VTKRENDERINGVR_EXPORT vtkVRRenderWindow : public vtkOpenGLRenderWindow
{
public:
  enum
  {
    PhysicalToWorldMatrixModified = vtkCommand::UserEvent + 200
  };

  enum
  {
    LeftEye = 0,
    RightEye
  };

  vtkTypeMacro(vtkVRRenderWindow, vtkOpenGLRenderWindow);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Create an interactor to control renderers in this window.
   * Must be overriden to instantiate a specific interactor depending on the API
   */
  vtkRenderWindowInteractor* MakeRenderWindowInteractor() override = 0;

  ///@{
  /**
   * Get the frame buffers used for rendering
   */
  GLuint GetLeftResolveBufferId() { return this->FramebufferDescs[LeftEye].ResolveFramebufferId; }
  GLuint GetRightResolveBufferId() { return this->FramebufferDescs[RightEye].ResolveFramebufferId; }
  void GetRenderBufferSize(int& width, int& height)
  {
    width = this->Size[0];
    height = this->Size[1];
  }
  ///@}

  /**
   * Get the index corresponding to the tracked device.
   * Used to retrieve models and poses.
   */
  virtual uint32_t GetTrackedDeviceIndexForDevice(vtkEventDataDevice idx, uint32_t index = 0) = 0;

  ///@{
  /**
   * Get the VRModel corresponding to the tracked device
   */
  vtkVRModel* GetTrackedDeviceModel(vtkEventDataDevice idx);
  virtual vtkVRModel* GetTrackedDeviceModel(uint32_t idx);
  ///@}

  ///@{
  /**
   * Get the pose matrix corresponding to the tracked device.
   */
  vtkMatrix4x4* GetTrackedDevicePose(vtkEventDataDevice idx);
  vtkMatrix4x4* GetTrackedDevicePose(uint32_t idx);
  ///@}

  /**
   * Initialize the HMD to World setting and camera settings so
   * that the VR world view most closely matched the view from
   * the provided camera. This method is useful for initialing
   * a VR world from an existing on screen window and camera.
   * The Renderer and its camera must already be created and
   * set when this is called.
   */
  virtual void InitializeViewFromCamera(vtkCamera* cam);

  /**
   * Get the world matrix corresponding to the given pose.
   */
  void ConvertPoseToWorldMatrix(vtkMatrix4x4* pose, vtkMatrix4x4* poseMatrixWorld);

  ///@{
  /**
   * Set/get physical coordinate system in world coordinate system.
   *
   * View direction is the -Z axis of the physical coordinate system
   * in world coordinate system.
   * \sa SetPhysicalViewUp, \sa SetPhysicalTranslation,
   * \sa SetPhysicalScale, \sa SetPhysicalToWorldMatrix
   */
  virtual void SetPhysicalViewDirection(double, double, double);
  virtual void SetPhysicalViewDirection(double[3]);
  vtkGetVector3Macro(PhysicalViewDirection, double);
  ///@}

  ///@{
  /**
   * Set/get physical coordinate system in world coordinate system.
   *
   * View up is the +Y axis of the physical coordinate system
   * in world coordinate system.
   * \sa SetPhysicalViewDirection, \sa SetPhysicalTranslation,
   * \sa SetPhysicalScale, \sa SetPhysicalToWorldMatrix
   */
  virtual void SetPhysicalViewUp(double, double, double);
  virtual void SetPhysicalViewUp(double[3]);
  vtkGetVector3Macro(PhysicalViewUp, double);
  ///@}

  ///@{
  /**
   * Set/get physical coordinate system in world coordinate system.
   *
   * Position of the physical coordinate system origin
   * in world coordinates.
   * \sa SetPhysicalViewDirection, \sa SetPhysicalViewUp,
   * \sa SetPhysicalScale, \sa SetPhysicalToWorldMatrix
   */
  virtual void SetPhysicalTranslation(double, double, double);
  virtual void SetPhysicalTranslation(double[3]);
  vtkGetVector3Macro(PhysicalTranslation, double);
  ///@}

  ///@{
  /**
   * Set/get physical coordinate system in world coordinate system.
   *
   * Ratio of distance in world coordinate and physical and system
   * (PhysicalScale = distance_World / distance_Physical).
   * Example: if world coordinate system is in mm then
   * PhysicalScale = 1000.0 makes objects appear in real size.
   * PhysicalScale = 100.0 makes objects appear 10x larger than real size.
   */
  virtual void SetPhysicalScale(double);
  vtkGetMacro(PhysicalScale, double);
  ///@}

  /**
   * Set physical to world transform matrix. Members calculated and set from the matrix:
   * \sa PhysicalViewDirection, \sa PhysicalViewUp, \sa PhysicalTranslation, \sa PhysicalScale
   * The x axis scale is used for \sa PhysicalScale
   */
  void SetPhysicalToWorldMatrix(vtkMatrix4x4* matrix);

  /**
   * Get physical to world transform matrix. Members used to calculate the matrix:
   * \sa PhysicalViewDirection, \sa PhysicalViewUp, \sa PhysicalTranslation, \sa PhysicalScale
   */
  void GetPhysicalToWorldMatrix(vtkMatrix4x4* matrix);

  /**
   * Add a renderer to the list of renderers.
   */
  void AddRenderer(vtkRenderer*) override;

  /**
   * Begin the rendering process.
   */
  void Start() override;

  /**
   * Initialize the rendering window.
   */
  void Initialize() override;

  /**
   * Finalize the rendering window.  This will shutdown all system-specific
   * resources. After having called this, it should be possible to destroy
   * a window that was used for a SetWindowId() call without any ill effects.
   */
  void Finalize() override;

  /**
   * Make this windows OpenGL context the current context.
   */
  void MakeCurrent() override;

  /**
   * Release the current context.
   */
  void ReleaseCurrent() override;

  /**
   * Tells if this window is the current OpenGL context for the calling thread.
   */
  bool IsCurrent() override;

  /**
   * Get report of capabilities for the render window
   */
  const char* ReportCapabilities() override { return "VR System"; }

  /**
   * Is this render window using hardware acceleration? 0-false, 1-true
   */
  vtkTypeBool IsDirect() override { return 1; }

  /**
   * Check to see if a mouse button has been pressed or mouse wheel activated.
   * All other events are ignored by this method.
   * Maybe should return 1 always?
   */
  vtkTypeBool GetEventPending() override { return 0; }

  /**
   * Get the current size of the screen in pixels.
   */
  int* GetScreenSize() override;

  ///@{
  /**
   * Set the size of the window in screen coordinates in pixels.
   * This resizes the operating system's window and redraws it.
   *
   * If the size has changed, this method will fire
   * vtkCommand::WindowResizeEvent.
   */
  void SetSize(int width, int height) override;
  void SetSize(int a[2]) override { this->SetSize(a[0], a[1]); }
  ///@}

  ///@{
  /**
   * Implement required virtual functions.
   */
  void* GetGenericDisplayId() override { return (void*)this->HelperWindow->GetGenericDisplayId(); }
  void* GetGenericWindowId() override { return (void*)this->HelperWindow->GetGenericWindowId(); }
  void* GetGenericParentId() override { return (void*)nullptr; }
  void* GetGenericContext() override { return (void*)this->HelperWindow->GetGenericContext(); }
  void* GetGenericDrawable() override { return (void*)this->HelperWindow->GetGenericDrawable(); }
  ///@}

  /**
   * Does this render window support OpenGL? 0-false, 1-true
   */
  int SupportsOpenGL() override { return 1; }

  /**
   * Overridden to not release resources that would interfere with an external
   * application's rendering. Avoiding round trip.
   */
  void Render() override;

  ///@{
  /**
   * Set/Get the window to use for the openGL context
   */
  vtkGetObjectMacro(HelperWindow, vtkOpenGLRenderWindow);
  void SetHelperWindow(vtkOpenGLRenderWindow* val);
  ///@}

  /**
   * Get the state object used to keep track of OpenGL state.
   */
  vtkOpenGLState* GetState() override;

  /**
   * Free up any graphics resources associated with this window
   * a value of nullptr means the context may already be destroyed
   */
  void ReleaseGraphicsResources(vtkWindow*) override;

  /**
   * Render the controller and base station models
   */
  virtual void RenderModels() = 0;

  /**
   * Store in \p poseMatrixWorld the pose matrix in world coordinate from an event data device.
   * Return true if the pose is valid, else false.
   */
  virtual bool GetPoseMatrixWorldFromDevice(
    vtkEventDataDevice device, vtkMatrix4x4* poseMatrixWorld);

  ///@{
  /**
   * When on the camera will track the HMD position.
   * On is the default.
   */
  vtkSetMacro(TrackHMD, bool);
  vtkGetMacro(TrackHMD, bool);
  ///@}

  //@{
  /**
   * Set/Get the visibility of the base stations. Defaults to false
   */
  vtkGetMacro(BaseStationVisibility, bool);
  vtkSetMacro(BaseStationVisibility, bool);
  vtkBooleanMacro(BaseStationVisibility, bool);
  //@}

  /**
   * Update the HMD pose.
   */
  virtual void UpdateHMDMatrixPose(){};

protected:
  vtkVRRenderWindow();
  ~vtkVRRenderWindow() override;

  struct FramebufferDesc
  {
    GLuint ResolveFramebufferId = 0;
    GLuint ResolveColorTextureId = 0;
    GLuint ResolveDepthTextureId = 0;
  };

  void CreateAWindow() override {}
  void DestroyWindow() override {}

  /**
   * Attempt to get the size of the display from the API
   * and store it in this->Size.
   * If succeed, returns true, else false.
   */
  virtual bool GetSizeFromAPI() = 0;

  virtual std::string GetWindowTitleFromAPI() { return "VTK - VR"; }

  virtual bool CreateFramebuffers(uint32_t viewCount = 2) = 0;
  void RenderFramebuffer(FramebufferDesc& framebufferDesc);

  bool TrackHMD = true;

  // One per view (typically one per eye)
  std::vector<FramebufferDesc> FramebufferDescs;

  // These vectors must be resized in subclass with the maximum number of devices
  std::vector<vtkSmartPointer<vtkVRModel>> TrackedDeviceToRenderModel;
  std::vector<vtkNew<vtkMatrix4x4>> TrackedDevicePoses;
  uint32_t InvalidDeviceIndex = UINT32_MAX;

  // used in computing the pose
  vtkNew<vtkTransform> HMDTransform;
  // -Z axis of the Physical to World matrix
  double PhysicalViewDirection[3] = { 0.0, 0.0, -1.0 };
  // Y axis of the Physical to World matrix
  double PhysicalViewUp[3] = { 0.0, 1.0, 0.0 };
  // Inverse of the translation component of the Physical to World matrix, in mm
  double PhysicalTranslation[3] = { 0.0, 0.0, 0.0 };
  // Scale of the Physical to World matrix
  double PhysicalScale = 1.0;

  bool BaseStationVisibility = false;

  vtkOpenGLRenderWindow* HelperWindow;

private:
  vtkVRRenderWindow(const vtkVRRenderWindow&) = delete;
  void operator=(const vtkVRRenderWindow&) = delete;
};

#endif
