/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWin32WebGPURenderWindow.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkWin32WebGPURenderWindow.h"

#include "vtkObjectFactory.h"
#include "vtkRendererCollection.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWin32WebGPURenderWindow);

const std::string vtkWin32WebGPURenderWindow::DEFAULT_BASE_WINDOW_NAME =
  "Visualization Toolkit - Win32WebGPU #";

//-------------------------------------------------------------------------------------------------
vtkWin32WebGPURenderWindow::vtkWin32WebGPURenderWindow()
{
  this->ApplicationInstance = nullptr;
  this->WindowId = 0;
  this->ParentId = 0;
  this->NextWindowId = 0;
  this->DeviceContext = (HDC)0;   // hsr
  this->MFChandledWindow = FALSE; // hsr
  this->StereoType = VTK_STEREO_CRYSTAL_EYES;
  this->CursorHidden = 0;
  this->Resizing = 0;
  this->Repositioning = 0;

  this->WindowIdReferenceCount = 0;

  this->SetWindowName(DEFAULT_BASE_WINDOW_NAME.c_str());
}

//-------------------------------------------------------------------------------------------------
vtkWin32WebGPURenderWindow::~vtkWin32WebGPURenderWindow()
{
  // close-down all system-specific drawing resources
  this->Finalize();

  vtkRenderer* ren;
  vtkCollectionSimpleIterator rit;
  this->Renderers->InitTraversal(rit);
  while ((ren = this->Renderers->GetNextRenderer(rit)))
  {
    ren->SetRenderWindow(nullptr);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Next Window Id: " << this->NextWindowId << "\n";
  os << indent << "Window Id: " << this->WindowId << "\n";
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::CleanUpRenderers()
{
  // tell each of the renderers that this render window/graphics context
  // is being removed (the RendererCollection is removed by vtkRenderWindow's
  // destructor)
  this->ReleaseGraphicsResources(this);
}

//-------------------------------------------------------------------------------------------------
LRESULT APIENTRY vtkWin32WebGPURenderWindow::WndProc(
  HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LRESULT res;

  vtkWin32WebGPURenderWindow* me =
    (vtkWin32WebGPURenderWindow*)vtkGetWindowLong(hWnd, sizeof(vtkLONG));

  if (me && me->GetReferenceCount() > 0)
  {
    me->Register(me);
    res = me->MessageProc(hWnd, message, wParam, lParam);
    me->UnRegister(me);
  }
  else
  {
    res = DefWindowProc(hWnd, message, wParam, lParam);
  }

  return res;
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetWindowName(const char* _arg)
{
  vtkWindow::SetWindowName(_arg);
  if (this->WindowId)
  {
    std::wstring wname = vtksys::Encoding::ToWide(this->WindowName);
    SetWindowTextW(this->WindowId, wname.c_str());
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetIcon(vtkImageData* img)
{
  int dim[3];
  img->GetDimensions(dim);

  int nbComp = img->GetNumberOfScalarComponents();

  if (img->GetScalarType() != VTK_UNSIGNED_CHAR || dim[2] != 1 || nbComp < 3 || nbComp > 4)
  {
    vtkErrorMacro(
      "Icon image should be 2D, have 3 or 4 components, and its type must be unsigned char.");
    return;
  }

  unsigned char* imgScalars = static_cast<unsigned char*>(img->GetScalarPointer());

  std::vector<unsigned char> pixels(nbComp * dim[0] * dim[1]);

  // Convert vtkImageData buffer to HBITMAP.
  // We need to flip Y and swap R and B channel
  for (int col = 0; col < dim[1]; col++)
  {
    for (int line = 0; line < dim[0]; line++)
    {
      unsigned char* inPixel = imgScalars + nbComp * ((dim[0] - col - 1) * dim[1] + line); // flip Y
      unsigned char* outPixel = pixels.data() + nbComp * (col * dim[1] + line);
      outPixel[0] = inPixel[2]; // swap R and B channel
      outPixel[1] = inPixel[1];
      outPixel[2] = inPixel[0]; // swap R and B channel
      outPixel[3] = inPixel[3];
    }
  }

  HBITMAP bmp = CreateBitmap(dim[0], dim[1], 1, nbComp * 8, pixels.data());

  HDC dc = GetDC(NULL);
  HBITMAP bmpMask = CreateCompatibleBitmap(dc, dim[0], dim[1]);

  ICONINFO ii;
  ii.fIcon = TRUE;
  ii.hbmMask = bmpMask;
  ii.hbmColor = bmp;

  HICON icon = CreateIconIndirect(&ii);

  SendMessage(this->WindowId, WM_SETICON, ICON_BIG, (LPARAM)icon);

  DeleteObject(bmpMask);
  DeleteObject(bmp);
  DestroyIcon(icon);
  ReleaseDC(NULL, dc);
}

//-------------------------------------------------------------------------------------------------
vtkTypeBool vtkWin32WebGPURenderWindow::GetEventPending()
{
  MSG msg;
  if (PeekMessage(&msg, this->WindowId, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE))
  {
    if (msg.message == WM_MOUSEMOVE)
    {
      PeekMessage(&msg, this->WindowId, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE);
    }
    if ((msg.message == WM_LBUTTONDOWN) || (msg.message == WM_RBUTTONDOWN) ||
      (msg.message == WM_MBUTTONDOWN) || (msg.message == WM_MOUSEWHEEL))
    {
      return 1;
    }
  }

  return 0;
}

//-------------------------------------------------------------------------------------------------
bool vtkWin32WebGPURenderWindow::InitializeFromCurrentContext()
{
  this->Superclass::InitializeFromCurrentContext();
  // TODO: Initialize the window/display parameters
  return false;
}

//-------------------------------------------------------------------------------------------------
namespace
{
void AdjustWindowRectForBorders(
  HWND hwnd, DWORD style, const int x, const int y, const int width, const int height, RECT& r)
{
  if (!style && hwnd)
  {
    style = GetWindowLong(hwnd, GWL_STYLE);
  }
  r.left = x;
  r.top = y;
  r.right = r.left + width;
  r.bottom = r.top + height;
  BOOL result = AdjustWindowRect(&r, style, FALSE);
  if (!result)
  {
    vtkGenericWarningMacro("AdjustWindowRect failed, error: " << GetLastError());
  }
}
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetSize(int width, int height)
{
  if ((this->Size[0] != width) || (this->Size[1] != height))
  {
    this->Superclass::SetSize(width, height);

    if (this->Interactor)
    {
      this->Interactor->SetSize(width, height);
    }

    if (!this->UseOffScreenBuffers)
    {
      if (!this->Resizing)
      {
        this->Resizing = 1;

        if (this->ParentId)
        {
          SetWindowExtEx(this->DeviceContext, width, height, nullptr);
          SetViewportExtEx(this->DeviceContext, width, height, nullptr);
          SetWindowPos(this->WindowId, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
        }
        else
        {
          RECT r;
          AdjustWindowRectForBorders(this->WindowId, 0, 0, 0, width, height, r);
          SetWindowPos(this->WindowId, HWND_TOP, 0, 0, r.right - r.left, r.bottom - r.top,
            SWP_NOMOVE | SWP_NOZORDER);
        }

        this->Resizing = 0;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetPosition(int x, int y)
{
  if ((this->Position[0] != x) || (this->Position[1] != y))
  {
    this->Modified();
    this->Position[0] = x;
    this->Position[1] = y;
    if (this->Mapped)
    {
      if (!this->Repositioning)
      {
        this->Repositioning = 1;

        SetWindowPos(this->WindowId, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        this->Repositioning = 0;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------
// End the rendering process and display the image.
void vtkWin32WebGPURenderWindow::Frame(void)
{
  this->MakeCurrent();
  this->Superclass::Frame();

  if (!this->AbortRender && this->DoubleBuffer && this->SwapBuffers)
  {
    // If this check is not enforced, we crash in offscreen rendering
    if (this->DeviceContext && !this->UseOffScreenBuffers)
    {
      // use global scope to get Win32 API SwapBuffers and not be
      // confused with this->SwapBuffers
      ::SwapBuffers(this->DeviceContext);
      vtkDebugMacro(<< " SwapBuffers\n");
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::VTKRegisterClass()
{
  // has the class been registered ?
  WNDCLASSA wndClass;
  if (!GetClassInfoA(this->ApplicationInstance, "vtkWebGPU", &wndClass))
  {
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wndClass.lpfnWndProc = vtkWin32WebGPURenderWindow::WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.hInstance = this->ApplicationInstance;
    wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = "vtkWebGPU";
    // vtk doesn't use the first extra vtkLONG's worth of bytes,
    // but app writers may want them, so we provide them. VTK
    // does use the second vtkLONG's worth of bytes of extra space.
    wndClass.cbWndExtra = 2 * sizeof(vtkLONG);
    RegisterClassA(&wndClass);
  }
}

//-------------------------------------------------------------------------------------------------
vtkTypeBool vtkWin32WebGPURenderWindow::IsDirect()
{
  if (!this->DeviceContext)
  {
    return 0;
  }

  int pixelFormat = GetPixelFormat(this->DeviceContext);
  PIXELFORMATDESCRIPTOR pfd;

  DescribePixelFormat(this->DeviceContext, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

  return (pfd.dwFlags & PFD_GENERIC_FORMAT) ? 0 : 1;
}

//-------------------------------------------------------------------------------------------------
const char* vtkWin32WebGPURenderWinodw::ReportCapabilities()
{
  if (!this->DisplayContext)
  {
    return "no device context";
  }
  // TODO: Ask WebGPU to
  // return this->Capabilities;
  return "";
}

//-------------------------------------------------------------------------------------------------
LRESULT vtkWin32WebGPURenderWindow::MessageProc(
  HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_CREATE:
    {
      // nothing to be done here, opengl is initialized after the call to
      // create now
      return 0;
    }
    case WM_DESTROY:
      if (this->DeviceContext)
      {
        ReleaseDC(this->WindowId, this->DeviceContext);
        this->DeviceContext = nullptr;
        this->WindowId = nullptr;
      }
      return 0;
    case WM_SIZE:
      /* track window size changes */
      if (this->WindowId)
      {
        this->SetSize((int)LOWORD(lParam), (int)HIWORD(lParam));
        return 0;
      }
      break;
    case WM_PALETTECHANGED:
      /* realize palette if this is *not* the current window */
      // if (this->ContextId && this->Palette && (HWND)wParam != hWnd)
      // {
      //   SelectPalette(this->DeviceContext, this->OldPalette, FALSE);
      //   UnrealizeObject(this->Palette);
      //   this->OldPalette = SelectPalette(this->DeviceContext, this->Palette, FALSE);
      //   RealizePalette(this->DeviceContext);
      //   this->Render();
      // }
      break;
    case WM_QUERYNEWPALETTE:
      /* realize palette if this is the current window */
      // if (this->ContextId && this->Palette)
      // {
      //   SelectPalette(this->DeviceContext, this->OldPalette, FALSE);
      //   UnrealizeObject(this->Palette);
      //   this->OldPalette = SelectPalette(this->DeviceContext, this->Palette, FALSE);
      //   RealizePalette(this->DeviceContext);
      //   this->Render();
      //   return TRUE;
      // }
      break;
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      if (this->DisplayId)
      {
        this->Render();
      }
      EndPaint(hWnd, &ps);
      return 0;
    }
    break;
    case WM_ERASEBKGND:
      return TRUE;
    case WM_SETCURSOR:
      if (HTCLIENT == LOWORD(lParam))
      {
        this->SetCurrentCursor(this->GetCurrentCursor());
        return TRUE;
      }
      break;
    default:
      this->InvokeEvent(vtkCommand::RenderWindowMessageEvent, &message);
      break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::InitializeApplication()
{
  // get the application instance if we don't have one already
  if (!this->ApplicationInstance)
  {
    // if we have a parent window get the app instance from it
    if (this->ParentId)
    {
      this->ApplicationInstance = (HINSTANCE)vtkGetWindowLong(this->ParentId, vtkGWL_HINSTANCE);
    }
    else
    {
      this->ApplicationInstance = GetModuleHandle(nullptr); /*AfxGetInstanceHandle();*/
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetShowWindow(bool val)
{
  if (val == this->ShowWindow)
  {
    return;
  }

  if (this->WindowId)
  {
    ::ShowWindow(this->WindowId, val ? SW_SHOW : SW_HIDE);
    this->Mapped = val;
  }
  this->Superclass::SetShowWindow(val);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::CreateAWindow()
{
  this->VTKRegisterClass();

  if (this->WindowIdReferenceCount == 0)
  {
    if (!this->WindowId)
    {
      this->DeviceContext = 0;

      if (this->GetWindowName() == DEFAULT_BASE_WINDOW_NAME)
      {
        static int count = 1;
        this->SetWindowName((DEFAULT_BASE_WINDOW_NAME + std::to_string(count++)).c_str());
      }

      int x = this->Position[0];
      int y = this->Position[1];
      int height = ((this->Size[1] > 0) ? this->Size[1] : 300);
      int width = ((this->Size[0] > 0) ? this->Size[0] : 300);

      std::wstring wname = vtksys::Encoding::ToWide(this->WindowName);
      /* create window */
      if (this->ParentId)
      {
        this->WindowId = CreateWindowW(L"vtkWebGPU", wname.c_str(),
          WS_CHILD | WS_CLIPCHILDREN /*| WS_CLIPSIBLINGS*/, x, y, width, height, this->ParentId,
          nullptr, this->ApplicationInstance, nullptr);
      }
      else
      {
        DWORD style;
        if (this->Borders)
        {
          style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN /*| WS_CLIPSIBLINGS*/;
        }
        else
        {
          style = WS_POPUP | WS_CLIPCHILDREN /*| WS_CLIPSIBLINGS*/;
        }
        RECT r;
        AdjustWindowRectForBorders(0, style, x, y, width, height, r);
        this->WindowId = CreateWindowW(L"vtkWebGPU", wname.c_str(), style, x, y, r.right - r.left,
          r.bottom - r.top, nullptr, nullptr, this->ApplicationInstance, nullptr);
      }

      if (!this->WindowId)
      {
        vtkErrorMacro("Could not create window, error:  " << GetLastError());
        return;
      }
      // extract the create info

      /* display window */
      if (this->ShowWindow)
      {
        ::ShowWindow(this->WindowId, SW_SHOW);
      }
      // UpdateWindow(this->WindowId);
      this->OwnWindow = 1;
      vtkSetWindowLong(this->WindowId, sizeof(vtkLONG), (intptr_t)this);
    }
    if (!this->DeviceContext)
    {
      this->DeviceContext = GetDC(this->WindowId);
    }

    // wipe out any existing display lists
    this->ReleaseGraphicsResources(this);
    this->Mapped = 1;
    this->WindowIdReferenceCount = 1;
  }
  else
  {
    ++this->WindowIdReferenceCount;
  }
}

//-------------------------------------------------------------------------------------------------
// Initialize the window for rendering.
void vtkWin32WebGPURenderWindow::WindowInitialize()
{
  // create our own window if not already set
  this->OwnWindow = 0;
  if (!this->MFChandledWindow)
  {
    this->InitializeApplication();
    this->CreateAWindow();
  }

  // tell our renderers about us
  vtkRenderer* ren;
  for (this->Renderers->InitTraversal(); (ren = this->Renderers->GetNextItem());)
  {
    ren->SetRenderWindow(nullptr);
    ren->SetRenderWindow(this);
  }
}

//-------------------------------------------------------------------------------------------------
// Initialize the rendering window.
void vtkWin32WebGPURenderWindow::Initialize(void)
{
  // make sure we haven't already been initialized
  if (!this->WindowId && !this->ApplicationInstance)
  {
    this->WindowInitialize();
  }

  if (this->WGPUInit())
  {
    wgpu::SurfaceDescriptorFromXlibWindow winSurfDesc;
    winSurfDesc.hwnd = this->GetWindowId();
    winSurfDesc.hinstance = this->ApplicationInstance;
    this->Surface = vtkWGPUContext::CreateSurface(winSurfDesc);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::Finalize(void)
{
  if (this->IsInitialized())
  {
    this->WGPUFinalize();
  }

  if (this->CursorHidden)
  {
    this->ShowCursor();
  }

  this->DestroyWindow();
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::DestroyWindow()
{
  if (this->WindowIdReferenceCount > 0)
  {
    --this->WindowIdReferenceCount;
    if (this->WindowIdReferenceCount == 0)
    {
      if (this->WindowId)
      {
        ReleaseDC(this->WindowId, this->DeviceContext);
        // can't set WindowId=nullptr, needed for DestroyWindow
        this->DeviceContext = nullptr;

        // clear the extra data before calling destroy
        vtkSetWindowLong(this->WindowId, sizeof(vtkLONG), (vtkLONG)0);
        if (this->OwnWindow)
        {
          ::DestroyWindow(this->WindowId); // windows api
          this->WindowId = 0;
        }
        this->Mapped = 0;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------
// Get the current size of the window.
int* vtkWin32WebGPURenderWindow::GetSize(void)
{
  // if we aren't mapped then just call super
  if (this->WindowId && !this->UseOffScreenBuffers)
  {
    RECT rect;

    // Find the current window size
    if (GetClientRect(this->WindowId, &rect))
    {
      this->Size[0] = rect.right;
      this->Size[1] = rect.bottom;
    }
    else
    {
      this->Size[0] = 0;
      this->Size[1] = 0;
    }
  }

  return this->Superclass::GetSize();
}

//-------------------------------------------------------------------------------------------------
// Get the size of the whole screen.
int* vtkWin32WebGPURenderWindow::GetScreenSize(void)
{
  HDC hDC = ::GetDC(nullptr);
  if (hDC)
  {
    // This technique yields the screen size of the primary monitor
    // only in a multi-monitor configuration...
    this->ScreenSize[0] = ::GetDeviceCaps(hDC, HORZRES);
    this->ScreenSize[1] = ::GetDeviceCaps(hDC, VERTRES);
    ::ReleaseDC(nullptr, hDC);
  }
  else
  {
    // This technique gets the "work area" (the whole screen except
    // for the bit covered by the Windows task bar) -- use it as a
    // fallback if there's an error calling GetDC.
    RECT rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

    this->ScreenSize[0] = rect.right - rect.left;
    this->ScreenSize[1] = rect.bottom - rect.top;
  }

  return this->ScreenSize;
}

//-------------------------------------------------------------------------------------------------
// Get the position in screen coordinates of the window.
int* vtkWin32WebGPURenderWindow::GetPosition(void)
{
  // if we aren't mapped then just return the ivar
  if (!this->Mapped)
  {
    return this->Position;
  }

  //  Find the current window position
  RECT rect;
  GetWindowRect(this->WindowId, &rect);
  this->Position[0] = rect.left;
  this->Position[1] = rect.top;

  return this->Position;
}

//-------------------------------------------------------------------------------------------------
// Change the window to fill the entire screen.
void vtkWin32WebGPURenderWindow::SetFullScreen(vtkTypeBool arg)
{
  if (this->FullScreen == arg)
  {
    return;
  }

  if (!this->Mapped)
  {
    this->PrefFullScreen();
    return;
  }

  // set the mode
  this->FullScreen = arg;
  if (this->FullScreen <= 0)
  {
    this->Position[0] = this->OldScreen[0];
    this->Position[1] = this->OldScreen[1];
    this->Size[0] = this->OldScreen[2];
    this->Size[1] = this->OldScreen[3];
    this->Borders = this->OldScreen[4];
  }
  else
  {
    // if window already up get its values
    if (this->WindowId)
    {
      const int* temp = this->GetPosition();
      this->OldScreen[0] = temp[0];
      this->OldScreen[1] = temp[1];

      this->OldScreen[4] = this->Borders;
      this->PrefFullScreen();
    }
  }

  // remap the window
  this->WindowRemap();

  this->Modified();
}

//-------------------------------------------------------------------------------------------------
// Set the variable that indicates that we want a stereo capable window
// be created. This method can only be called before a window is realized.
//
void vtkWin32WebGPURenderWindow::SetStereoCapableWindow(vtkTypeBool capable)
{
  if (this->WindowId == 0)
  {
    this->Superclass::SetStereoCapableWindow(capable);
  }
  else
  {
    vtkWarningMacro(<< "Requesting a StereoCapableWindow must be performed "
                    << "before the window is realized, i.e. before a render.");
  }
}

//-------------------------------------------------------------------------------------------------
// Set the preferred window size to full screen.
void vtkWin32WebGPURenderWindow::PrefFullScreen()
{
  const int* size = this->GetScreenSize();

  // don't show borders
  this->Borders = 0;

  RECT r;
  AdjustWindowRectForBorders(this->WindowId, 0, 0, 0, size[0], size[1], r);

  // use full screen
  this->Position[0] = 0;
  this->Position[1] = 0;
  this->Size[0] = r.right - r.left;
  this->Size[1] = r.bottom - r.top;
}

//-------------------------------------------------------------------------------------------------
// Remap the window.
void vtkWin32WebGPURenderWindow::WindowRemap()
{
  // close everything down
  this->Finalize();

  // set the default windowid
  this->WindowId = this->NextWindowId;
  this->NextWindowId = 0;

  // and set it up!
  this->Initialize();
}

//-------------------------------------------------------------------------------------------------
// Get the window id.
HWND vtkWin32WebGPURenderWindow::GetWindowId()
{
  vtkDebugMacro(<< "Returning WindowId of " << this->WindowId << "\n");

  return this->WindowId;
}

//-------------------------------------------------------------------------------------------------
// Set the window id to a pre-existing window.
void vtkWin32WebGPURenderWindow::SetWindowId(HWND arg)
{
  vtkDebugMacro(<< "Setting WindowId to " << arg << "\n");

  if (arg != this->WindowId)
  {
    this->WindowId = arg;
    this->DeviceContext = 0;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetDisplayId(void* arg)
{
  this->DeviceContext = (HDC)arg;
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetDeviceContext(HDC arg)
{
  this->DeviceContext = arg;
  this->MFChandledWindow = TRUE;
}

//-------------------------------------------------------------------------------------------------
// Sets the HWND id of the window that WILL BE created.
void vtkWin32WebGPURenderWindow::SetParentInfo(const char* info)
{
  int tmp;

  sscanf(info, "%i", &tmp);

  this->ParentId = (HWND)tmp;
  vtkDebugMacro(<< "Setting ParentId to " << this->ParentId << "\n");
}

//-------------------------------------------------------------------------------------------------
// Set the window id to a pre-existing window.
void vtkWin32WebGPURenderWindow::SetParentId(HWND arg)
{
  vtkDebugMacro(<< "Setting ParentId to " << arg << "\n");

  this->ParentId = arg;
}

//-------------------------------------------------------------------------------------------------
// Set the window id of the new window once a WindowRemap is done.
void vtkWin32WebGPURenderWindow::SetNextWindowId(HWND arg)
{
  vtkDebugMacro(<< "Setting NextWindowId to " << arg << "\n");

  this->NextWindowId = arg;
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetNextWindowId(void* arg)
{
  this->SetNextWindowId((HWND)arg);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::HideCursor()
{
  if (this->CursorHidden)
  {
    return;
  }
  this->CursorHidden = 1;

  ::ShowCursor(!this->CursorHidden);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::ShowCursor()
{
  if (!this->CursorHidden)
  {
    return;
  }
  this->CursorHidden = 0;

  ::ShowCursor(!this->CursorHidden);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetCursorPosition(int x, int y)
{
  const int* size = this->GetSize();

  POINT point;
  point.x = x;
  point.y = size[1] - y - 1;

  if (ClientToScreen(this->WindowId, &point))
  {
    SetCursorPos(point.x, point.y);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetCursorPosition(int x, int y)
{
  const int* size = this->GetSize();

  POINT point;
  point.x = x;
  point.y = size[1] - y - 1;

  if (ClientToScreen(this->WindowId, &point))
  {
    SetCursorPos(point.x, point.y);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32WebGPURenderWindow::SetCurrentCursor(int shape)
{
  if (this->InvokeEvent(vtkCommand::CursorChangedEvent, &shape))
  {
    return;
  }
  this->Superclass::SetCurrentCursor(shape);
  LPCTSTR cursorName = 0;
  switch (shape)
  {
    case VTK_CURSOR_DEFAULT:
    case VTK_CURSOR_ARROW:
      cursorName = IDC_ARROW;
      break;
    case VTK_CURSOR_SIZENE:
    case VTK_CURSOR_SIZESW:
      cursorName = IDC_SIZENESW;
      break;
    case VTK_CURSOR_SIZENW:
    case VTK_CURSOR_SIZESE:
      cursorName = IDC_SIZENWSE;
      break;
    case VTK_CURSOR_SIZENS:
      cursorName = IDC_SIZENS;
      break;
    case VTK_CURSOR_SIZEWE:
      cursorName = IDC_SIZEWE;
      break;
    case VTK_CURSOR_SIZEALL:
      cursorName = IDC_SIZEALL;
      break;
    case VTK_CURSOR_HAND:
#if (WINVER >= 0x0500)
      cursorName = IDC_HAND;
#else
      cursorName = IDC_ARROW;
#endif
      break;
    case VTK_CURSOR_CROSSHAIR:
      cursorName = IDC_CROSS;
      break;
    case VTK_CURSOR_CUSTOM:
      cursorName = static_cast<LPCTSTR>(this->GetCursorFileName());
      break;
    default:
      cursorName = 0;
      break;
  }

  if (cursorName)
  {
    UINT fuLoad = LR_SHARED | LR_DEFAULTSIZE;
    if (shape == VTK_CURSOR_CUSTOM)
    {
      fuLoad |= LR_LOADFROMFILE;
    }
    HANDLE cursor = LoadImage(0, cursorName, IMAGE_CURSOR, 0, 0, fuLoad);
    if (!cursor)
    {
      vtkErrorMacro("failed to load requested cursor shape " << GetLastError());
    }
    else
    {
      SetCursor((HCURSOR)cursor);
      DestroyCursor((HCURSOR)cursor);
    }
  }
}

//-------------------------------------------------------------------------------------------------
bool vtkWin32WebGPURenderWindow::DetectDPI()
{
  this->SetDPI(GetDeviceCaps(this->DeviceContext, LOGPIXELSY));
  return true;
}

VTK_ABI_NAMESPACE_END
