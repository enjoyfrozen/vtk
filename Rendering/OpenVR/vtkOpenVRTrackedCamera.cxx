/*=========================================================================

Program:   Visualization Toolkit

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkCallbackCommand.h"
#include "vtkOpenVRRenderer.h"
#include "vtkOpenVRTrackedCamera.h"
#include "vtkObjectFactory.h" //vtkStandardNewMacro
#include "vtkOpenVRRenderWindowInteractor.h" //rw->MakeCurrent()
#include "vtkPolygon.h"

vtkStandardNewMacro(vtkOpenVRTrackedCamera);

//------------------------------------------------------------------------------
vtkOpenVRTrackedCamera::vtkOpenVRTrackedCamera()
{
  this->pHMD = nullptr;
  this->VRTrackedCamera = 0;

  this->CameraFrameWidth = 0;
  this->CameraFrameHeight = 0;
  this->CameraFrameBufferSize = 0;
  this->CameraFrameBuffer = nullptr;

  this->LastFrameSequence = 0;

  this->RenderWindow = nullptr;
  this->SetEnabled(true);
  this->SetDrawingEnabled(false);

  this->RenderCallbackCommand = vtkCallbackCommand::New();
  this->RenderCallbackCommand->SetClientData(this);
  this->RenderCallbackCommand->SetCallback(vtkOpenVRTrackedCamera::RenderEvent);
  this->RenderCallbackCommand->SetPassiveObserver(1);

  this->Pass = vtkPassThrough::New();

  this->Renderer = nullptr;

  this->FrameType = vr::VRTrackedCameraFrameType_Distorted; //Default

  this->TrackedCameraActor = vtkActor::New();

  this->BackgroundRenderer = vtkOpenVRRenderer::New();

  this->SourceImage = vtkImageData::New();
  memset(&this->CurrentFrameHeader, 0, sizeof(this->CurrentFrameHeader));

  vtkDebugMacro(<< "vtkOpenVRTrackedCamera() Build : "
    << __DATE__ << " " << __TIME__);
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::SetFrameType(int type)
{
  switch (type)
  {
  case 0:
    this->FrameType = vr::VRTrackedCameraFrameType_Distorted;
    break;
  case 1:
    this->FrameType = vr::VRTrackedCameraFrameType_Undistorted;
    break;
  case 2:
    this->FrameType = vr::VRTrackedCameraFrameType_MaximumUndistorted;
    break;
  }
}

//------------------------------------------------------------------------------
int vtkOpenVRTrackedCamera::GetFrameType()
{
  return this->FrameType;
}

//------------------------------------------------------------------------------
std::string vtkOpenVRTrackedCamera::GetFrameTypeAsString(int type)
{
  switch ((int)type)
  {
  case 0:
    return "VRTrackedCameraFrameType_Distorted";
    break;
  case 1:
    return "VRTrackedCameraFrameType_Undistorted";
    break;
  case 2:
    return "VRTrackedCameraFrameType_MaximumUndistorted";
    break;
  default:
    return "Unknown Type";
    break;
  }
}

//------------------------------------------------------------------------------
std::string vtkOpenVRTrackedCamera::GetFrameTypeAsString()
{
  return GetFrameTypeAsString(this->FrameType);
}

//------------------------------------------------------------------------------
vtkOpenVRTrackedCamera::~vtkOpenVRTrackedCamera()
{
  this->Delete();
  this->pHMD = nullptr;
  this->VRTrackedCamera = nullptr;
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::Initialize(vtkOpenVRRenderWindow *rw)
{
  vtkDebugMacro(<< "Initialize Tracked Camera");
  this->RenderWindow = rw;
  this->pHMD = rw->GetHMD();

  this->VRTrackedCamera = vr::VRTrackedCamera();
  if (!this->VRTrackedCamera)
  {
    vtkDebugMacro(<< "Unable to get tracked camera interface");
    return;
  }

  bool bHasCamera = false;
  vr::EVRTrackedCameraError nCameraError =
    this->VRTrackedCamera->HasCamera(vr::k_unTrackedDeviceIndex_Hmd, &bHasCamera);

  if (nCameraError != vr::VRTrackedCameraError_None || !bHasCamera)
  {
    vtkDebugMacro(<< "No Tracked Camera Available "
      << VRTrackedCamera->GetCameraErrorNameFromEnum(nCameraError))
      return;
  }

  // Accessing the FW description is just a further check to ensure camera communication
  // is valid as expected
  vr::ETrackedPropertyError propertyError;
  char buffer[1024];

  pHMD->GetStringTrackedDeviceProperty(
    vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraFirmwareDescription_String,
    buffer, sizeof(buffer), &propertyError);
  if (propertyError != vr::TrackedProp_Success)
  {
    vtkDebugMacro(<< "failed to get tracked camera firmware description");
    return;
  }
  pHMD->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd,
    vr::Prop_CameraFirmwareDescription_String, buffer, sizeof(buffer), &propertyError);
  if (propertyError != vr::TrackedProp_Success)
  {
    vtkDebugMacro(<< "Error initializing the front camera");
    return;
  }

  vtkDebugMacro(<< "Camera Firmware " << buffer);
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::BuildRepresentation()
{
  // StartVideoPreview before Init the CameraPreviewImage
  // to init it with Correct FrameWidth and FrameHeight
  this->StartVideoPreview();
  this->InitPreview(this->CameraFrameWidth, this->CameraFrameHeight);

  //Read the image
  // Get a port on the vtkImageData *CameraPreviewImage->GetSourceImage()
  this->Pass->RemoveAllInputs();
  this->Pass->AddInputData(this->GetSourceImage());

  vr::HmdVector2_t pFocalLength, pCenter;
  vr::EVRTrackedCameraError nCameraError =
    this->VRTrackedCamera->GetCameraIntrinsics(vr::k_unTrackedDeviceIndex_Hmd,
    vr::VRTrackedCameraFrameType_Undistorted, &pFocalLength, &pCenter);
  double fx, fy;
  fx = pFocalLength.v[0];
  fy = pFocalLength.v[1];

  // Setup  points
  //Multiplication by 1/fx or 1/fy (camera calibration)
  // so the scale can be set to one
  double wid = (double)this->CameraFrameWidth * 1 / fx;
  double hei = (double)this->CameraFrameHeight * 1 / fy;
  double a = wid / 2.0;
  double b = hei / 2.0;
  double c = -50.0;

  // Create the polygon
  int n = 4;
  vtkPoints* Points = vtkPoints::New();
  vtkPolygon* Polygon  = vtkPolygon::New();
  vtkCellArray* Polygons = vtkCellArray::New();
  vtkPolyData* PolygonPolyData = vtkPolyData::New();
  vtkPolyDataMapper* Mapper = vtkPolyDataMapper::New();
  vtkTexture* Texture = vtkTexture::New();
  vtkFloatArray* TextureCoordinates = vtkFloatArray::New();
  Polygon->GetPointIds()->SetNumberOfIds(2 * (n + 1)); //make a quad

  TextureCoordinates->SetNumberOfComponents(2);
  TextureCoordinates->SetName("TextureCoordinates");

  for (int i = 0; i <= n; i++)
  {
    double fac = (double)i;
    Points->InsertNextPoint(-a + 2.0*fac*a / n, -b, 0.0);
    float tuple[3] = { fac / n, 0.0, 0.0 };
    TextureCoordinates->InsertNextTuple(tuple);
  }
  for (int i = 0; i <= n; i++)
  {
    double fac = (double)i;
    Points->InsertNextPoint(a - 2.0 * fac*a / n, b, 0.0);
    float tuple[3] = { 1.0 - fac / n, 1.0, 0.0 };
    TextureCoordinates->InsertNextTuple(tuple);
  }

  for (int i = 0; i < 2 * (n + 1); i++)
    Polygon->GetPointIds()->SetId(i, i);

  //Add the polygon to a list of polygons
  Polygons->InsertNextCell(Polygon);

  PolygonPolyData->SetPoints(Points); //geometry
  PolygonPolyData->SetPolys(Polygons); //topology
  PolygonPolyData->GetPointData()->SetTCoords(TextureCoordinates);

  Mapper->SetInputData(PolygonPolyData);
  Texture->SetInputConnection(this->Pass->GetOutputPort());

  this->TrackedCameraActor->SetMapper(Mapper);
  this->TrackedCameraActor->SetTexture(Texture);

  if (this->DrawingEnabled)
  {
    vtkOpenVRCamera *actCamera =
      vtkOpenVRCamera::SafeDownCast(this->Renderer->GetActiveCamera());

    this->BackgroundRenderer->SetBackground(0.2, 0.3, 0.4);
    this->RenderWindow->SetNumberOfLayers(2);
    this->RenderWindow->AddRenderer(BackgroundRenderer);
    this->BackgroundRenderer->SetLayer(0);
    this->BackgroundRenderer->InteractiveOff();
    this->Renderer->SetLayer(1);

    this->BackgroundRenderer->AddActor(TrackedCameraActor);

    // Set up the background camera to fill the renderer with the image
    double origin[3];
    double spacing[3];
    int extent[6];
    this->GetSourceImage()->GetOrigin(origin);
    this->GetSourceImage()->GetSpacing(spacing);
    this->GetSourceImage()->GetExtent(extent);

    /*Camera to be used on the background renderer to not shift the view*/
    vtkSmartPointer<vtkOpenVRCamera> camera =
      vtkSmartPointer<vtkOpenVRCamera>::New();
    camera->ParallelProjectionOn();
    double xc = origin[0] + 0.5*(extent[0] + extent[1])*spacing[0];
    double yc = origin[1] + 0.5*(extent[2] + extent[3])*spacing[1];
    double yd = (extent[3] - extent[2] + 1)*spacing[1];
    double d = actCamera->GetDistance();
    camera->SetParallelScale(0.5*yd);
    camera->SetFocalPoint(xc, yc, 0.0);
    camera->SetPosition(xc, yc, d);

    this->BackgroundRenderer->SetActiveCamera(camera);
  }
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::RenderEvent(vtkObject* object,
  unsigned long event,
  void* clientdata,
  void* calldata)
{
  vtkOpenVRTrackedCamera *self =
    static_cast<vtkOpenVRTrackedCamera *>(clientdata);

  vtkEventData *ed = static_cast<vtkEventData *>(calldata);

  if (self->Enabled)
  {
    self->DisplayRefreshTimeout();
    self->UpdateDisplayPosition();
    self->TrackedCameraActor->SetVisibility(self->DrawingEnabled);
  }
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::UpdateDisplayPosition()
{
  if (!this->Enabled)
    return;

  vtkOpenVRRenderWindow* vRrenWin =
    vtkOpenVRRenderWindow::SafeDownCast(this->Renderer->GetRenderWindow());
  vtkOpenVRRenderWindowInteractor* vRinteractor =
    vtkOpenVRRenderWindowInteractor::SafeDownCast(this->Renderer->GetRenderWindow()->GetInteractor());

  if (!vRrenWin || !vRinteractor)
  {
    return;
  }

  if (this->Renderer && vRrenWin && vRinteractor)
  {
    //Update physical scale
    double physicalScale = vRinteractor->GetPhysicalScale();

    int hmdIdx = static_cast<int>(vtkEventDataDevice::HeadMountedDisplay);
    const vr::TrackedDevicePose_t &tdPose =
      vRrenWin->GetTrackedDevicePose(hmdIdx);

    double pos[3];
    double ppos[3];
    double wxyz[4];
    double wdir[3];
    vRinteractor->ConvertPoseToWorldCoordinates(tdPose, pos, wxyz, ppos, wdir);

    vtkOpenVRCamera *actCamera =
      vtkOpenVRCamera::SafeDownCast(this->Renderer->GetActiveCamera());
    if (!actCamera)
    {
      return;
    }

    double *ori = actCamera->GetOrientationWXYZ();
    vtkSmartPointer<vtkTransform> tr =
      vtkSmartPointer<vtkTransform>::New();
    tr->Identity();
    tr->RotateWXYZ(-ori[0], ori[1], ori[2], ori[3]);

    double *frameForward = actCamera->GetDirectionOfProjection();
    double *hmdUpWc = tr->TransformDoubleVector(0.0, 1.0, 0.0);
    double frameRight[3];
    vtkMath::Cross(hmdUpWc, frameForward, frameRight);
    vtkMath::Normalize(frameRight);

    double theta = 10.0;
    double thetaRad = theta * vtkMath::Pi() / 180.0;
    double A = std::cos(thetaRad);
    double B = std::sin(thetaRad);

    hmdUpWc[0] = hmdUpWc[0] * A + frameForward[0] * B;
    hmdUpWc[1] = hmdUpWc[1] * A + frameForward[1] * B;
    hmdUpWc[2] = hmdUpWc[2] * A + frameForward[2] * B;

    frameForward[0] = -hmdUpWc[0] * B + frameForward[0] * A;
    frameForward[1] = -hmdUpWc[1] * B + frameForward[1] * A;
    frameForward[2] = -hmdUpWc[2] * B + frameForward[2] * A;

    vtkMath::Normalize(frameForward);
    vtkMath::Normalize(hmdUpWc);

    double offsetScreen[3] = { 1.2 ,0.025 , -0.035 };

    double framePosition[3] = {
      pos[0],
      pos[1],
      pos[2]
    };

    //apply offset along the frameForward axis
    framePosition[0] += offsetScreen[0] * frameForward[0];
    framePosition[1] += offsetScreen[0] * frameForward[1];
    framePosition[2] += offsetScreen[0] * frameForward[2];

    //apply offset along the frameRight axis
    framePosition[0] += offsetScreen[1] * frameRight[0];
    framePosition[1] += offsetScreen[1] * frameRight[1];
    framePosition[2] += offsetScreen[1] * frameRight[2];

    //apply offset along the frameUp axis
    framePosition[0] += offsetScreen[2] * hmdUpWc[0];
    framePosition[1] += offsetScreen[2] * hmdUpWc[1];
    framePosition[2] += offsetScreen[2] * hmdUpWc[2];

    this->TrackedCameraActor->SetPosition(framePosition);
    this->TrackedCameraActor->SetOrientation(tr->GetOrientation());

    this->TrackedCameraActor->SetScale(1);
  }
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::StartVideoPreview()
{
  vtkDebugMacro(<< "StartVideoPreview()");
  // Allocate for camera frame buffer requirements
  uint32_t nCameraFrameBufferSize = 0;

  if (this->VRTrackedCamera->GetCameraFrameSize(vr::k_unTrackedDeviceIndex_Hmd,
    this->FrameType, &this->CameraFrameWidth, &this->CameraFrameHeight,
    &nCameraFrameBufferSize) != vr::VRTrackedCameraError_None)
  {
    vtkDebugMacro(<< "GetCameraFrameBounds() Failed");
    return;
  }

  if (nCameraFrameBufferSize && nCameraFrameBufferSize != this->CameraFrameBufferSize)
  {
    delete[] this->CameraFrameBuffer;
    this->CameraFrameBufferSize = nCameraFrameBufferSize;
    this->CameraFrameBuffer = new uint8_t[this->CameraFrameBufferSize];
    memset(this->CameraFrameBuffer, 0, this->CameraFrameBufferSize);
  }

  this->LastFrameSequence = 0;

  this->VRTrackedCamera->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd,
    &this->VRTrackedCameraHandle);
  if (this->VRTrackedCameraHandle == INVALID_TRACKED_CAMERA_HANDLE)
  {
    vtkDebugMacro(<< "AcquireVideoStreamingService() Failed");
    return;
  }
  this->TrackedCameraActor->VisibilityOn();
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::StopVideoPreview()
{
  vtkDebugMacro(<< "Stop Video Preview");

  if (!this->VRTrackedCamera)
  {
    vtkDebugMacro(<< "Tracked Camera nullptr");
  }

  if (this->TrackedCameraActor->GetVisibility())
  {
    this->TrackedCameraActor->VisibilityOff();
  }
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::DisplayRefreshTimeout()
{
  if (!this->Enabled || !this->VRTrackedCamera || !this->VRTrackedCameraHandle)
  {
    return;
  }

  //get the frame header only
  vr::CameraVideoStreamFrameHeader_t frameHeader;
  vr::EVRTrackedCameraError nCameraError =
    this->VRTrackedCamera->GetVideoStreamFrameBuffer(this->VRTrackedCameraHandle,
      this->FrameType, nullptr, 0, &frameHeader, sizeof(frameHeader));
  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    return;
  }

  if (frameHeader.nFrameSequence == this->LastFrameSequence)
  {
    // frame hasn't changed yet, nothing to do
    return;
  }

  // Frame has changed, do the more expensive frame buffer copy
  nCameraError =
    this->VRTrackedCamera->GetVideoStreamFrameBuffer(this->VRTrackedCameraHandle,
      this->FrameType, this->CameraFrameBuffer, this->CameraFrameBufferSize,
      &frameHeader, sizeof(frameHeader));
  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    return;
  }
  //TO DO : There should be a less costing way to get the image
  //using the frame buffer without having to copy it in a vtkImageData to represent it.
  //this->VRTrackedCamera->GetVideoStreamTextureGL()
  this->LastFrameSequence = frameHeader.nFrameSequence;
  this->SetFrameImage(this->CameraFrameBuffer,
    this->CameraFrameWidth, this->CameraFrameHeight, &frameHeader);

  this->Pass->Modified();
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::SetEnabled(bool val)
{
  if (val == this->Enabled)
  {
    return;
  }

  this->Enabled = val;
  if (!this)
  {
    return;
  }
  if (!this->Enabled)
  {
    this->StopVideoPreview();
  }
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::SetDrawingEnabled(bool enable)
{
  if (enable == this->DrawingEnabled)
  {
    return;
  }
  this->DrawingEnabled = enable;
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::SetRenderer(vtkRenderer *ren)
{
  vtkOpenVRRenderer *vrRen = vtkOpenVRRenderer::SafeDownCast(ren);
  if (!vrRen)
  {
    std::cerr << "Not a VR Renderer " << std::endl;
    return;
  }
  if (vrRen == this->Renderer)
  {
    return;
  }

  if (this->Renderer)
  {
    vtkOpenVRRenderWindowInteractor *interactor =
      vtkOpenVRRenderWindowInteractor::SafeDownCast(
        this->Renderer->GetRenderWindow()->GetInteractor());
    if (!interactor)
    {
      return;
    }
    interactor->RemoveObserver(this->ObserverTag);
  }

  this->Renderer = vrRen;
  if (this->Renderer)
  {
    vtkOpenVRRenderWindowInteractor *interactor =
      vtkOpenVRRenderWindowInteractor::SafeDownCast(
        this->Renderer->GetRenderWindow()->GetInteractor());
    if (!interactor)
    {
      return;
    }
    this->ObserverTag = interactor->AddObserver(vtkCommand::Move3DEvent, this->RenderCallbackCommand, 10.0);
  }

  this->Modified();
}

//----------------------------------------------------------------------------
vtkRenderer* vtkOpenVRTrackedCamera::GetRenderer()
{
  return this->Renderer;
}

//----------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::InitPreview(uint32_t nFrameWidth, uint32_t nFrameHeight)
{
  this->SourceImage->SetDimensions((int)nFrameWidth, (int)nFrameHeight, 1);
  this->SourceImage->SetSpacing(1.0, 1.0, 1.0);
  this->SourceImage->SetOrigin(0.0, 0.0, 0.0);
  this->SourceImage->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
}

//----------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::SetFrameImage(const uint8_t *pFrameImage, uint32_t nFrameWidth, uint32_t nFrameHeight, const vr::CameraVideoStreamFrameHeader_t *pFrameHeader)
{
  if (pFrameHeader)
  {
    this->CurrentFrameHeader = *pFrameHeader;
  }

  if (pFrameImage && nFrameWidth && nFrameHeight)
  {
    if (this->SourceImage &&
      ((uint32_t)this->SourceImage->GetDimensions()[0] != nFrameWidth ||
      (uint32_t)this->SourceImage->GetDimensions()[1] != nFrameHeight))
    {
      //dimension changed
      this->SourceImage->Delete();
      this->SourceImage = nullptr;
    }

    if (!this->SourceImage)
    {
      // allocate to expected dimensions
      this->SourceImage = vtkImageData::New();
      this->SourceImage->SetDimensions((int)nFrameWidth, (int)nFrameHeight, 1);
      this->SourceImage->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
    }
    for (int y = (int)nFrameHeight - 1; y >= 0; y--)
    {
      for (int x = 0; x < (int)nFrameWidth; x++)
      {
        int* dims = SourceImage->GetDimensions();
        unsigned char* pixel = static_cast<unsigned char*>(this->SourceImage->GetScalarPointer(x, y, 0));

        if (!pixel)
        {
          vtkDebugMacro(<< "Pixel Null, check for errors");
          return;
        }
        pixel[0] = pFrameImage[0];
        pixel[1] = pFrameImage[1];
        pixel[2] = pFrameImage[2];
        pFrameImage += 4;
      }
    }
  }
}
