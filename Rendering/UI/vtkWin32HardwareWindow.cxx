/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWin32HardwareWindow.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkWin32HardwareWindow.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include <assert.h>

//=================================================================================================
VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkWin32HardwareWindow);

//-------------------------------------------------------------------------------------------------
vtkWin32HardwareWindow::vtkWin32HardwareWindow()
{
  this->ApplicationInstance = nullptr;
  this->WindowId = 0;
  this->ParentId = 0;
  this->CursorHidden = 0;
}

//-------------------------------------------------------------------------------------------------
vtkWin32HardwareWindow::~vtkWin32HardwareWindow()
{
  if (this->WindowId)
  {
    this->Destroy();
  }
  if (this->CursorHidden)
  {
    this->ShowCursor();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Window Id: " << this->WindowId << "\n";
}

//-------------------------------------------------------------------------------------------------
HINSTANCE vtkWin32HardwareWindow::GetApplicationInstance()
{
  return this->ApplicationInstance;
}

//-------------------------------------------------------------------------------------------------
HWND vtkWin32HardwareWindow::GetWindowId()
{
  return this->WindowId;
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::SetDisplayId(void* arg)
{
  this->ApplicationInstance = (HINSTANCE)(arg);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::SetWindowId(void* arg)
{
  this->WindowId = (HWND)(arg);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::SetParentId(void* arg)
{
  this->ParentId = (HWND)(arg);
}

//-------------------------------------------------------------------------------------------------
void* vtkWin32HardwareWindow::GetGenericDisplayId()
{
  return this->ApplicationInstance;
}

//-------------------------------------------------------------------------------------------------
void* vtkWin32HardwareWindow::GetGenericWindowId()
{
  return this->WindowId;
}

//-------------------------------------------------------------------------------------------------
void* vtkWin32HardwareWindow::GetGenericParentId()
{
  return this->ParentId;
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
void vtkWin32HardwareWindow::Create()
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

  // has the class been registered ?
  WNDCLASSA wndClass;
  if (!GetClassInfoA(this->ApplicationInstance, "vtkOpenGL", &wndClass))
  {
    wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wndClass.lpfnWndProc = DefWindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.hInstance = this->ApplicationInstance;
    wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = "vtkOpenGL";
    // vtk doesn't use the first extra vtkLONG's worth of bytes,
    // but app writers may want them, so we provide them. VTK
    // does use the second vtkLONG's worth of bytes of extra space.
    wndClass.cbWndExtra = 2 * sizeof(vtkLONG);
    RegisterClassA(&wndClass);
  }

  if (!this->WindowId)
  {
    int x = this->Position[0];
    int y = this->Position[1];
    int height = ((this->Size[1] > 0) ? this->Size[1] : 300);
    int width = ((this->Size[0] > 0) ? this->Size[0] : 300);

    std::wstring wname = vtksys::Encoding::ToWide(this->WindowName);
    /* create window */
    if (this->ParentId)
    {
      this->WindowId =
        CreateWindowA(L"vtkWebGPU", wname.c_str(), WS_CHILD | WS_CLIPCHILDREN /*| WS_CLIPSIBLINGS*/,
          x, y, width, height, this->ParentId, nullptr, this->ApplicationInstance, nullptr);
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
      this->WindowId = CreateWindowA(L"vtkWebGPU", wname.c_str(), style, x, y, r.right - r.left,
        r.bottom - r.top, nullptr, nullptr, this->ApplicationInstance, nullptr);
    }

    if (!this->WindowId)
    {
      vtkGenericWarningMacro("Could not create window, error:  " << GetLastError());
      return;
    }
    // extract the create info

    /* display window */
    if (this->ShowWindow)
    {
      ::ShowWindow(this->WindowId, SW_SHOW);
    }
    // UpdateWindow(this->WindowId);
    // vtkSetWindowLong(this->WindowId, sizeof(vtkLONG), (intptr_t)this);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::Destroy()
{
  ::DestroyWindow(this->WindowId); // windows api
  this->WindowId = 0;
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::SetSize(int x, int y)
{
  static bool resizing = false;
  if ((this->Size[0] != x) || (this->Size[1] != y))
  {
    this->Superclass::SetSize(x, y);

    if (!this->UseOffScreenBuffers)
    {
      if (!resizing)
      {
        resizing = true;

        if (this->ParentId)
        {
          SetWindowExtEx(GetDC(this->WindowId), x, y, nullptr);
          SetViewportExtEx(GetDC(this->WindowId), x, y, nullptr);
          SetWindowPos(this->WindowId, HWND_TOP, 0, 0, x, y, SWP_NOMOVE | SWP_NOZORDER);
        }
        else
        {
          RECT r;
          AdjustWindowRectForBorders(this->WindowId, 0, 0, 0, x, y, r);
          SetWindowPos(this->WindowId, HWND_TOP, 0, 0, r.right - r.left, r.bottom - r.top,
            SWP_NOMOVE | SWP_NOZORDER);
        }
        resizing = false;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::SetPosition(int x, int y)
{
  static bool resizing = false;

  if ((this->Position[0] != x) || (this->Position[1] != y))
  {
    this->Modified();
    this->Position[0] = x;
    this->Position[1] = y;
    if (this->Mapped)
    {
      if (!resizing)
      {
        resizing = true;

        SetWindowPos(this->WindowId, HWND_TOP, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        resizing = false;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::HideCursor()
{
  if (this->CursorHidden)
  {
    return;
  }
  this->CursorHidden = 1;

  ::ShowCursor(!this->CursorHidden);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::ShowCursor()
{
  if (!this->CursorHidden)
  {
    return;
  }
  this->CursorHidden = 0;

  ::ShowCursor(!this->CursorHidden);
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::SetCursorPosition(int x, int y)
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
void vtkWin32HardwareWindow::SetCurrentCursor(int shape)
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
void vtkWin32HardwareWindow::SetWindowName(const char* _arg)
{
  vtkWindow::SetWindowName(_arg);
  if (this->WindowId)
  {
    std::wstring wname = vtksys::Encoding::ToWide(this->WindowName);
    SetWindowTextW(this->WindowId, wname.c_str());
  }
}

//-------------------------------------------------------------------------------------------------
void vtkWin32HardwareWindow::SetIcon(vtkImageData* img)
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
VTK_ABI_NAMESPACE_END
