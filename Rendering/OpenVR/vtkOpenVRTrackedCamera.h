/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkOpenVRTrackedCamera.h

--Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
--All rights reserved.
--See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
* @class   vtkOpenVRTrackedCamera
* @brief   OpenVR tracked camera
*
* vtkOpenVRFrontCamera support for VR Front camera (or TrackedCamera) :
* Allows the user to draw what is viewed by the front camera.
* The drawing is performed on a screen always in front of the user.
* It also permits to get the vtkImageData by accessing
* GetCameraPreviewImage()->GetSourceImage().
*
* EnableOn() activate the camera
* DrawingEnabledOn()/DrawingEnabledOff() set if the screen should be drawn or not
*
*/

#ifndef vtkopenvrtrackedcamera_h
#define vtkopenvrtrackedcamera_h

#include "vtkRenderingOpenVRModule.h" // For export macro
#include "vtkObject.h"

#include "vtkEventData.h" // for enums
#include "vtkFloatArray.h" //for texture coordinates
#include "vtkImageData.h"
#include "vtkOpenVRCamera.h" //To differentiate background camera when rendering video mode
#include "vtkPointData.h" //texture coordinates
#include "vtkPolyData.h" //representation of the video mode
#include "vtkPolyDataMapper.h"
#include "vtkTexture.h"

#include <openvr.h> //for ivars

class VTKRENDERINGOPENVR_EXPORT vtkOpenVRTrackedCamera : public vtkProp
{
public:
  static vtkOpenVRTrackedCamera *New();
  vtkTypeMacro(vtkOpenVRTrackedCamera, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override { this->Superclass::PrintSelf(os, indent); }

  /**
   * Initialize the tracked camera
   */
  virtual void Initialize(vtkOpenVRRenderWindow *rw);

  /**
 * Start Video Preview
 * Should only be used internaly
 */
  void StartVideoPreview();

  /**
  * Stop Video Preview
  * Should only be used internaly
  */
  void StopVideoPreview();

  /**
  * Update the position of the screen displaying what the front camera sees
  * Follow the user to be always in front (similar to what vtkFollower does)
  */
  void UpdateDisplayPosition();

  //@{
 /**
 * Set / get the state of the tracked camera.
 * This defines if the tracked camera is enabled.
 * SetEnabled is not defined with vtkSetMacro cause
 * it's calling to stopVideoPreview for memory use reason.
 */
  void SetEnabled(bool enabled);
  vtkGetMacro(Enabled, bool);
  vtkBooleanMacro(Enabled, bool);

  //@{
  /**
  * Set / get enable drawing of the video mode tracked camera.
  * This defines if the tracked camera is enabled to be
  * drawn with the video mode (square in front of the user).
  */
  //void SetDrawingEnabled(bool enabled);
  vtkSetMacro(DrawingEnabled, bool);
  vtkGetMacro(DrawingEnabled, bool);
  vtkBooleanMacro(DrawingEnabled, bool);

  //@{
  /**
  * Set / get enable acquisition of the video mode tracked
  * camera as an imageData.
  */
  //void SetImageData(const uint8_t *pFrameImage, uint32_t nFrameWidth, uint32_t nFrameHeight);
  vtkSetMacro(AcquireImageData, bool);
  vtkGetMacro(AcquireImageData, bool);
  vtkBooleanMacro(AcquireImageData, bool);

  /*Accessor to Camera texture as image data*/
  vtkGetMacro(FrameImageData, vtkImageData*);

  /**
  * Build representation of the video mode for tracked camera as a textured quad
  */
  void BuildRepresentation();

  /**
  * Accesor to the VRTrackedCamera
  */
  vr::IVRTrackedCamera* GetVRTrackedCamera() { return this->VRTrackedCamera; }

  //@{
  /**
  * Set / get the Renderer associated with TrackedCamera.
  */
  virtual void SetRenderer(vtkRenderer *ren);
  virtual vtkRenderer* GetRenderer();

  //@{
  /**
  * Set / get the Frame Type to be used.
  * 0 - Distorted
  * 1 - Undistorted
  * 2 - Maximum_Undistorted
  * For more indications about the differences, check OpenVR API Documentation
  */
  void SetFrameType(int type);
  int GetFrameType() const;

  /**
  *Return frame type as a string for user understanding
  *The type must be furnished as int
  */
  std::string GetFrameTypeAsString(int type) const;

  /**
  * Return the current frame type as a string for user understanding
  */
  std::string GetFrameTypeAsString() const;

  /**
  * Helper to access the texture as an imageData
  */
  void AcquireFrameAsImageData();

protected:
  vtkOpenVRTrackedCamera();
  ~vtkOpenVRTrackedCamera();

  /** Render event - used to update position of the TrackedCameraActor */
  static void RenderEvent(vtkObject* object, unsigned long event, void* clientdata, void* calldata);

  /*Callback command triggering render of TrackedCamera*/
  vtkCallbackCommand* RenderCallbackCommand;
  unsigned long ObserverTag;

  /** Vive System */
  vr::IVRSystem *pHMD;

  /** Pointer to the parent vtkOpenVRRenderWindow */
  vtkSmartPointer<vtkOpenVRRenderWindow> RenderWindow;

  /** The principal Scene renderer*/
  vtkOpenVRRenderer* Renderer;
  /** The background renderer to display tracked camera*/
  vtkOpenVRRenderer* BackgroundRenderer;

  /** Tracked Camera (or front camera) */
  vr::IVRTrackedCamera *VRTrackedCamera;

  /*Video Stream Texture Frame Width*/
  uint32_t				TextureFrameWidth;
  /*Video Stream Texture Frame Height*/
  uint32_t				TextureFrameHeight;
  /*Video Stream Texture Bounds (uMin,uMax,vMin,vMax)*/
  vr::VRTextureBounds_t TextureBounds;

  /*Camera Frame Width*/
  uint32_t				CameraFrameWidth;
  /*Camera Frame Height*/
  uint32_t				CameraFrameHeight;
  /*Camera Frame Buffer Size*/
  uint32_t        CameraFrameBufferSize;
  /*Frame BufferFor copy and storage in image data*/
  uint8_t					*CameraFrameBuffer;
  /*Store number of last frame sequence from the frame header*/
  uint32_t				LastFrameSequence;
  /*Texture handle*/
  vr::glUInt_t TextureGLId;
  /*Tracked Camera frame header*/
  vr::CameraVideoStreamFrameHeader_t FrameHeaderTextureGL;

  /**Number of components in the texture returned by OpenVR
  * - Distorted Frame Type has no alpha channel (3 components)
  * - Undistorted and MaximumUndistorted has a alpha channel (4 components)*/
  int NumberOfComponents;
  /** Type of frame from the Tracked Camera : distorted/Undistorted/MaximumUndistorted*/
  vr::EVRTrackedCameraFrameType FrameType;

  /** The tracked Camera has a unique TrackedCameraHandle_t
  * This handle is used to set attributes, receive events, (and render?).
  * These are several circumstances where the tracked camera isn't detected or invalid.
  * In those case the handle will be equal to INVALID_TRACKED_CAMERA_HANDLE */
  vr::TrackedCameraHandle_t VRTrackedCameraHandle;

  /** The actor representing the tracked camera*/
  vtkActor* TrackedCameraActor;
  /** Texture object to handle creation of vtk Texture from the openvr StreamTextureGL */
  vtkTextureObject* CameraTexture;
  /**Texture linking the texture object (openvr/opengl) to the actor*/
  vtkOpenGLTexture* TextureMap;

  /*Enable storage of the texture within an image data
  * Imply a GPU to CPU copy - Not efficient but a way to access to the
  * visual information*/
  bool AcquireImageData;

  /*Storage of the TrackedCamera Texture on the CPU*/
  vtkImageData* FrameImageData;

  /** Is the tracked camera is enabled */
  bool Enabled;

  /** The camera should be drawn into the HMD - Option for the user that may want to activate the camera but not to respresent it */
  bool DrawingEnabled;

private:
  vtkOpenVRTrackedCamera(const vtkOpenVRTrackedCamera&) = delete;
  void operator=(const vtkOpenVRTrackedCamera&) = delete;
};
#endif //vtkopenvrtrackedcamera_h
