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
#include "vtkOpenGLTexture.h"
#include "vtkOpenVRRenderer.h"
#include "vtkOpenVRTrackedCamera.h"
#include "vtkObjectFactory.h" //vtkStandardNewMacro
#include "vtkOpenVRRenderWindowInteractor.h" //rw->MakeCurrent()
#include "vtkPolygon.h"
#include "vtkProperty.h"
#include"vtkSmartPointer.h"
#include "vtkTextureObject.h"
#include "vtkRendererCollection.h"
#include "vtkInteractorStyle.h"
vtkStandardNewMacro(vtkOpenVRTrackedCamera);

//------------------------------------------------------------------------------
vtkOpenVRTrackedCamera::vtkOpenVRTrackedCamera()
{
  this->RenderCallbackCommand = nullptr;
  this->ObserverTag = 0;

  this->pHMD = nullptr;

  this->RenderWindow = nullptr;
  this->Renderer = nullptr;
  this->BackgroundRenderer = nullptr;

  this->VRTrackedCamera = nullptr;
  this->TextureFrameWidth = 0;
  this->TextureFrameHeight = 0;
  this->TextureBounds.uMax = 0.f;
  this->TextureBounds.uMin = 0.f;
  this->TextureBounds.vMax = 0.f;
  this->TextureBounds.vMin = 0.f;

  this->CameraFrameWidth = 0;
  this->CameraFrameHeight = 0;
  this->CameraFrameBufferSize = 0;

  this->NumberOfComponents = 0;
  this->FrameType = vr::EVRTrackedCameraFrameType::VRTrackedCameraFrameType_Distorted;

  this->VRTrackedCameraHandle = 0;
  this->TrackedCameraActor = nullptr;
  this->CameraTexture = nullptr;
  this->TextureMap = nullptr;

  this->Enabled = false;
  this->DrawingEnabled = false;

  this->TextureGLId = 0;
  this->FrameHeaderTextureGL = vr::CameraVideoStreamFrameHeader_t();

  this->FrameImageData = nullptr;
  this->AcquireImageData = false;
  this->CameraFrameBuffer = nullptr;
  this->LastFrameSequence = 0;
}

//------------------------------------------------------------------------------
vtkOpenVRTrackedCamera::~vtkOpenVRTrackedCamera()
{
  if (this->RenderCallbackCommand)
  {
    this->RenderCallbackCommand->Delete();
    this->RenderCallbackCommand = nullptr;
  }

  if (this->pHMD)
  {
    delete this->pHMD;
    this->pHMD = nullptr;
  }

  if (this->RenderWindow)
  {
    this->RenderWindow->Delete();
    this->RenderWindow = nullptr;
  }

  if (this->Renderer)
  {
    this->Renderer->Delete();
    this->Renderer = nullptr;
  }

  if (this->BackgroundRenderer)
  {
    this->BackgroundRenderer->Delete();
    this->BackgroundRenderer = nullptr;
  }

  if (this->VRTrackedCamera)
  {
    delete this->VRTrackedCamera;
    this->VRTrackedCamera = nullptr;
  }

  if (this->TrackedCameraActor)
  {
    this->TrackedCameraActor->Delete();
    this->TrackedCameraActor = nullptr;
  }
  if (this->CameraTexture)
  {
    this->CameraTexture->Delete();
    this->CameraTexture = nullptr;
  }
  if (this->TextureMap)
  {
    this->TextureMap->Delete();
    this->TextureMap = nullptr;
  }

  if (this->FrameImageData)
  {
    this->FrameImageData->Delete();
    this->FrameImageData = nullptr;
  }
  if (this->CameraFrameBuffer)
  {
    delete[] this->CameraFrameBuffer;
    this->CameraFrameBuffer = nullptr;
  }
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::Initialize(vtkOpenVRRenderWindow *renWin)
{
  vtkDebugMacro(<< "Initialize Tracked Camera");
  //Callback init
  this->RenderCallbackCommand = vtkCallbackCommand::New();
  this->RenderCallbackCommand->SetClientData(this);
  this->RenderCallbackCommand->SetCallback(vtkOpenVRTrackedCamera::RenderEvent);
  this->RenderCallbackCommand->SetPassiveObserver(1);

  this->SetFrameType(0); //Default

  //Context init and tracked camera interface creation
  if (!renWin)
  {
    return;
  }
  this->RenderWindow = vtkOpenVRRenderWindow::SafeDownCast(renWin);
  if (!this->RenderWindow)
  {
    return;
  }
  this->pHMD = renWin->GetHMD();
  this->VRTrackedCamera = vr::VRTrackedCamera();

  if (!this->VRTrackedCamera)
  {
    vtkDebugMacro(<< "Unable to get tracked camera interface");
    return;
  }
  bool hasCamera = false;
  vr::EVRTrackedCameraError nCameraError =
    this->VRTrackedCamera->HasCamera(vr::k_unTrackedDeviceIndex_Hmd, &hasCamera);

  if (nCameraError != vr::VRTrackedCameraError_None || !hasCamera)
  {
    vtkDebugMacro(<< "No Tracked Camera Available "
      << VRTrackedCamera->GetCameraErrorNameFromEnum(nCameraError))
      return;
  }

  // Accessing the FW description is just a further check to ensure camera
  // communication is valid as expected
  vr::ETrackedPropertyError propertyError;
  char buffer[1024];

  this->pHMD->GetStringTrackedDeviceProperty(
    vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraFirmwareDescription_String,
    buffer, sizeof(buffer), &propertyError);
  if (propertyError != vr::TrackedProp_Success)
  {
    vtkDebugMacro(<< "failed to get tracked camera firmware description");
    return;
  }

  this->pHMD->GetStringTrackedDeviceProperty(
    vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_CameraFirmwareDescription_String,
    buffer, sizeof(buffer), &propertyError);
  if (propertyError != vr::TrackedProp_Success)
  {
    vtkDebugMacro(<< "Error initializing the front camera");
    return;
  }

  vtkDebugMacro(<< "Camera Firmware " << buffer);

  nCameraError = this->VRTrackedCamera->GetVideoStreamTextureSize(vr::k_unTrackedDeviceIndex_Hmd,
    this->FrameType, &this->TextureBounds, &this->TextureFrameWidth, &this->TextureFrameHeight);
  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    vtkDebugMacro(<< "GetCameraFrameBounds() Failed");
    return;
  }

  //Initialize to current context so layers can be set next
  vtkOpenVRRenderer* ren =
    vtkOpenVRRenderer::SafeDownCast(this->RenderWindow->GetRenderers()->GetFirstRenderer());
  this->SetRenderer(ren);
  this->BuildRepresentation();
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::BuildRepresentation()
{

  vr::HmdVector2_t pFocalLength, pCenter;
  vr::EVRTrackedCameraError nCameraError =
    this->VRTrackedCamera->GetCameraIntrinsics(vr::k_unTrackedDeviceIndex_Hmd,
      vr::VRTrackedCameraFrameType_MaximumUndistorted, &pFocalLength, &pCenter);

  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    return;
  }

  double fx, fy;
  fx = pFocalLength.v[0];
  fy = pFocalLength.v[1];

  // Setup  points
  // Multiplication by 1/fx or 1/fy (camera intrinsics)
  // so the scale can be set to one
  double wid = (double)this->TextureFrameWidth * 1 / fx;
  double hei = (double)this->TextureFrameHeight * 1 / fy;

  // Create the polygon
  int n = 4;
  vtkPoints* points = vtkPoints::New();
  vtkPolygon* polygon = vtkPolygon::New();
  vtkCellArray* polygons = vtkCellArray::New();
  vtkPolyData* polygonPolyData = vtkPolyData::New();
  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  vtkFloatArray* textureCoordinates = vtkFloatArray::New();
  polygon->GetPointIds()->SetNumberOfIds(2 * (n + 1)); //make a quad

  textureCoordinates->SetNumberOfComponents(2);
  textureCoordinates->SetName("TextureCoordinates");

  points->InsertNextPoint(-wid / 2.0, hei / 2.0, 0.0);
  points->InsertNextPoint(wid / 2.0, hei / 2.0, 0.0);
  points->InsertNextPoint(wid / 2.0, -hei / 2.0, 0.0);
  points->InsertNextPoint(-wid / 2.0, -hei / 2.0, 0.0);

  polygon->GetPointIds()->SetNumberOfIds(4); //make a quad
  polygon->GetPointIds()->SetId(0, 0);
  polygon->GetPointIds()->SetId(1, 1);
  polygon->GetPointIds()->SetId(2, 2);
  polygon->GetPointIds()->SetId(3, 3);

  float tuple[3] = { this->TextureBounds.uMin, this->TextureBounds.vMin, 0.0 };
  textureCoordinates->InsertNextTuple(tuple);
  tuple[0] = this->TextureBounds.uMax; tuple[1] = this->TextureBounds.vMin; tuple[2] = 0.0;
  textureCoordinates->InsertNextTuple(tuple);
  tuple[0] = this->TextureBounds.uMax; tuple[1] = this->TextureBounds.vMax; tuple[2] = 0.0;
  textureCoordinates->InsertNextTuple(tuple);
  tuple[0] = this->TextureBounds.uMin; tuple[1] = this->TextureBounds.vMax; tuple[2] = 0.0;
  textureCoordinates->InsertNextTuple(tuple);

  //Add the polygon to a list of polygons
  polygons->InsertNextCell(polygon);

  polygonPolyData->SetPoints(points); //geometry
  polygonPolyData->SetPolys(polygons); //topology
  polygonPolyData->GetPointData()->SetTCoords(textureCoordinates);

  mapper->SetInputData(polygonPolyData);

  this->TrackedCameraActor = vtkActor::New();
  this->TrackedCameraActor->SetMapper(mapper);
  this->TrackedCameraActor->GetProperty()->SetSpecular(100.0);

  this->BackgroundRenderer = vtkOpenVRRenderer::New();

  this->BackgroundRenderer->SetBackground(0.2, 0.3, 0.4);
  this->RenderWindow->SetNumberOfLayers(2);
  this->RenderWindow->AddRenderer(BackgroundRenderer);
  this->BackgroundRenderer->SetLayer(0);
  this->BackgroundRenderer->InteractiveOff();
  this->Renderer->SetLayer(1);

  points->Delete();
  polygon->Delete();
  polygons->Delete();
  polygonPolyData->Delete();
  mapper->Delete();
  textureCoordinates->Delete();
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::RenderEvent(vtkObject* vtkNotUsed(object),
  unsigned long vtkNotUsed(event),
  void* clientdata,
  void* vtkNotUsed(calldata))
{
  vtkOpenVRTrackedCamera *self =
    static_cast<vtkOpenVRTrackedCamera *>(clientdata);

  if (self->Enabled)
  {
    self->TrackedCameraActor->SetVisibility(self->GetDrawingEnabled());
    if (self->DrawingEnabled)
    {
      self->UpdateDisplayPosition();
    }
    if (self->AcquireImageData)
    {
      self->AcquireFrameAsImageData();
    }
  }
}



//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::UpdateDisplayPosition()
{
  if (!this->Enabled)
    return;

  vtkOpenVRRenderWindowInteractor* vRinteractor =
    vtkOpenVRRenderWindowInteractor::SafeDownCast(this->RenderWindow->GetInteractor());

  if (!this->RenderWindow || !vRinteractor)
  {
    return;
  }

  if (this->Renderer && this->RenderWindow && vRinteractor)
  {
    int hmdIdx = static_cast<int>(vtkEventDataDevice::HeadMountedDisplay);
    const vr::TrackedDevicePose_t &tdPose =
      this->RenderWindow->GetTrackedDevicePose(hmdIdx);

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

    //Hard settings : emplacement of the tracked camera w.r.t to the vtk camera
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

  this->RenderWindow->MakeCurrent();

  vr::EVRTrackedCameraError nCameraError =
    this->VRTrackedCamera->AcquireVideoStreamingService(vr::k_unTrackedDeviceIndex_Hmd,
      &this->VRTrackedCameraHandle);

  if (nCameraError == vr::VRTrackedCameraError_None &&
    this->VRTrackedCameraHandle == INVALID_TRACKED_CAMERA_HANDLE)
  {
    vtkDebugMacro(<< "AcquireVideoStreamingService() Failed");
    return;
  }

  nCameraError = this->VRTrackedCamera->GetVideoStreamTextureGL(this->VRTrackedCameraHandle,
    this->FrameType, &this->TextureGLId,
    &this->FrameHeaderTextureGL, sizeof(this->FrameHeaderTextureGL));

  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    return;
  }
  if (!this->CameraTexture)
  {
    this->CameraTexture = vtkTextureObject::New();
  }
  this->CameraTexture->SetContext(this->RenderWindow);
  this->CameraTexture->Create2DFromHandle(
    this->TextureFrameWidth, this->TextureFrameHeight,
    this->NumberOfComponents, VTK_UNSIGNED_CHAR,
    this->TextureGLId);

  this->TextureMap = vtkOpenGLTexture::New();
  this->TextureMap->SetTextureObject(this->CameraTexture);
  this->TrackedCameraActor->SetTexture(this->TextureMap);
  this->BackgroundRenderer->AddActor(TrackedCameraActor);

  double origin[3];
  double spacing[3];
  int extent[6];

  vtkSmartPointer<vtkOpenVRCamera> camera = vtkSmartPointer<vtkOpenVRCamera>::New();
  camera->ParallelProjectionOn();
  this->BackgroundRenderer->SetActiveCamera(camera);

  //For image Data storage
  nCameraError = this->VRTrackedCamera->GetCameraFrameSize(
    vr::k_unTrackedDeviceIndex_Hmd, this->FrameType,
    &this->CameraFrameWidth, &this->CameraFrameHeight, &nCameraFrameBufferSize);
  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    vtkDebugMacro(<< "GetCameraFrameBounds() Failed");
    return;
  }

  //Resize the buffer if Frame size has changed
  if (nCameraFrameBufferSize && nCameraFrameBufferSize != this->CameraFrameBufferSize)
  {
    delete[] this->CameraFrameBuffer;
    this->CameraFrameBufferSize = nCameraFrameBufferSize;
    this->CameraFrameBuffer = new uint8_t[this->CameraFrameBufferSize];
    memset(this->CameraFrameBuffer, 0, this->CameraFrameBufferSize);
  }

}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::StopVideoPreview()
{
  vtkDebugMacro(<< "Stop Video Preview");

  if (!this->VRTrackedCamera)
  {
    vtkDebugMacro(<< "Tracked Camera nullptr");
  }

  vr::EVRTrackedCameraError nCameraError =
    this->VRTrackedCamera->ReleaseVideoStreamTextureGL(this->VRTrackedCameraHandle,
      this->TextureGLId);

  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    vtkDebugMacro(<< "ReleaseVideoStreamTextureGL() Failed in StopVideoPreview()");
    return;
  }

  nCameraError = this->VRTrackedCamera->ReleaseVideoStreamingService(this->VRTrackedCameraHandle);

  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    vtkDebugMacro(<< "ReleaseVideoStreamingService() Failed in StopVideoPreview()");
    return;
  }
}

//------------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::SetEnabled(bool val)
{
  if (val == this->Enabled)
  {
    return;
  }

  this->Enabled = val;
  if (this->Enabled)
  {
    this->StartVideoPreview();
  }
  else
  {
    this->StopVideoPreview();
  }
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
    this->ObserverTag = interactor->AddObserver(vtkCommand::Move3DEvent, this->RenderCallbackCommand, 0.0);
  }
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkOpenVRTrackedCamera::AcquireFrameAsImageData()
{
  if (!this->TextureMap)
  {
    return;
  }

  vr::CameraVideoStreamFrameHeader_t frameHeader;
  vr::EVRTrackedCameraError nCameraError = this->VRTrackedCamera->GetVideoStreamFrameBuffer(this->VRTrackedCameraHandle, this->FrameType, nullptr, 0, &frameHeader, sizeof(frameHeader));
  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    return;
  }

  if (frameHeader.nFrameSequence == this->LastFrameSequence)
  {
    // frame hasn't changed yet, nothing to do
    return;
  }

  nCameraError = this->VRTrackedCamera->GetVideoStreamFrameBuffer(this->VRTrackedCameraHandle, this->FrameType, this->CameraFrameBuffer, this->CameraFrameBufferSize, &frameHeader, sizeof(frameHeader));
  if (nCameraError != vr::VRTrackedCameraError_None)
  {
    return;
  }

  this->LastFrameSequence = frameHeader.nFrameSequence;

  //Frame Buffer copy to vtkImageData
  const uint8_t *pFrameImage = this->CameraFrameBuffer;

  if (this->FrameImageData
    && ((uint32_t)this->FrameImageData->GetDimensions()[0] != this->CameraFrameWidth ||
    (uint32_t)this->FrameImageData->GetDimensions()[1] != this->CameraFrameHeight))
  {
    this->FrameImageData->Delete();
    this->FrameImageData = nullptr;
  }

  if (!this->FrameImageData)
  {
    this->FrameImageData = vtkImageData::New();
    this->FrameImageData->SetDimensions(this->CameraFrameWidth, this->CameraFrameHeight, 1);
    this->FrameImageData->AllocateScalars(VTK_UNSIGNED_CHAR, this->NumberOfComponents);
  }
  for (int y = (int)CameraFrameHeight - 1; y >= 0; y--)
  {
    for (int x = 0; x < (int)CameraFrameWidth; x++)
    {
      int* dims = FrameImageData->GetDimensions();
      unsigned char* pixel = static_cast<unsigned char*>(this->FrameImageData->GetScalarPointer(x, y, 0));

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

//----------------------------------------------------------------------------
vtkRenderer* vtkOpenVRTrackedCamera::GetRenderer()
{
  return this->Renderer;
}

//------------------------------------------------------------------------------
int vtkOpenVRTrackedCamera::GetFrameType() const
{
  return this->FrameType;
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
  switch (this->FrameType)
  {
  case 0:
    this->NumberOfComponents = 3;
    break;
  case 1:
    this->NumberOfComponents = 3;
    break;
  case 2:
    this->NumberOfComponents = 4;
    break;
  }
}

//------------------------------------------------------------------------------
std::string vtkOpenVRTrackedCamera::GetFrameTypeAsString(int type) const
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
std::string vtkOpenVRTrackedCamera::GetFrameTypeAsString() const
{
  return GetFrameTypeAsString(this->FrameType);
}
