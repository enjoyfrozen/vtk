/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkXlibHardwareWindow.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkXlibHardwareWindow.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkRenderWindow.h"

// STL includes
#include <assert.h>

// Xlib includes
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#if VTK_HAVE_XCURSOR
#include <X11/Xcursor/Xcursor.h>
#endif

//-------------------------------------------------------------------------------------------------
template <int EventType>
int XEventTypeEquals(Display*, XEvent* event, XPointer winptr)
{
  return (event->type == EventType &&
    *(reinterpret_cast<Window*>(winptr)) == reinterpret_cast<XAnyEvent*>(event)->window);
}

//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkXlibHardwareWindow);

//-------------------------------------------------------------------------------------------------
vtkXlibHardwareWindow::vtkXlibHardwareWindow()
{
  this->ParentId = static_cast<Window>(0);
  this->OwnDisplay = 0;
  this->CursorHidden = 0;
  this->DisplayId = static_cast<Display*>(nullptr);
  this->WindowId = static_cast<Window>(0);
  this->ColorMap = static_cast<Colormap>(0);
  this->OwnWindow = 0;

  this->XCCrosshair = 0;
  this->XCArrow = 0;
  this->XCSizeAll = 0;
  this->XCSizeNS = 0;
  this->XCSizeWE = 0;
  this->XCSizeNE = 0;
  this->XCSizeNW = 0;
  this->XCSizeSE = 0;
  this->XCSizeSW = 0;
  this->XCHand = 0;
  this->XCCustom = 0;
}

//-------------------------------------------------------------------------------------------------
vtkXlibHardwareWindow::~vtkXlibHardwareWindow()
{
  if (this->WindowId && this->DisplayId && (this->OwnDisplay || this->OwnWindow))
  {
    this->Destroy();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ColorMap: " << this->ColorMap << "\n";
  os << indent << "OwnWindow: " << (this->OwnWindow ? "Y" : "N") << "\n";
  os << indent << "OwnDisplay: " << (this->OwnDisplay ? "Y" : "N") << "\n";
  os << indent << "FullScreen: " << (this->FullScreen ? "Y" : "N") << "\n";
}

//-------------------------------------------------------------------------------------------------
Display* vtkXlibHardwareWindow::GetDisplayId()
{
  return this->DisplayId;
}

//-------------------------------------------------------------------------------------------------
Window vtkXlibHardwareWindow::GetWindowId()
{
  return this->WindowId;
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::SetDisplayId(void* arg)
{
  this->DisplayId = (Display*)(arg);
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::SetWindowId(void* arg)
{
  vtkDebugMacro(<< "Setting WindowId to " << reinterpret_cast<void*>(arg) << "\n");
  this->WindowId = reinterpret_cast<Window>(arg);
  if (this->CursorHidden)
  {
    this->CursorHidden = 0;
    this->HideCursor();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::SetParentId(void* arg)
{
  vtkDebugMacro(<< "Setting ParentId to " << reinterpret_cast<void*>(arg) << "\n");
  this->ParentId = reinterpret_cast<Window>(arg);
}

//-------------------------------------------------------------------------------------------------
void* vtkXlibHardwareWindow::GetGenericDisplayId()
{
  return this->DisplayId;
}

//-------------------------------------------------------------------------------------------------
void* vtkXlibHardwareWindow::GetGenericWindowId()
{
  return reinterpret_cast<void*>(this->WindowId);
}

//-------------------------------------------------------------------------------------------------
void* vtkXlibHardwareWindow::GetGenericParentId()
{
  return reinterpret_cast<void*>(this->ParentId);
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::Create()
{
  XVisualInfo *v, matcher;
  XSetWindowAttributes attr;
  int x, y, width, height, nItems;
  XWindowAttributes winattr;
  XSizeHints xsh;
  XClassHint xch;

  xsh.flags = USSize;
  if ((this->Position[0] >= 0) && (this->Position[1] >= 0))
  {
    xsh.flags |= USPosition;
    xsh.x = static_cast<int>(this->Position[0]);
    xsh.y = static_cast<int>(this->Position[1]);
  }

  x = this->Position[0];
  y = this->Position[1];
  width = ((this->Size[0] > 0) ? this->Size[0] : 300);
  height = ((this->Size[1] > 0) ? this->Size[1] : 300);

  xsh.width = width;
  xsh.height = height;

  // get the default display connection
  if (!this->OpenDisplay())
  {
    abort();
  }

  attr.override_redirect = False;
  if (this->Borders == 0.0)
  {
    attr.override_redirect = True;
  }

  // create our own window ?
  this->OwnWindow = false;
  if (!this->WindowId)
  {
    v = this->GetDesiredVisualInfo();
    if (!v)
    {
      vtkErrorMacro(<< "Could not find a decent visual\n");
      abort();
    }
    this->ColorMap = XCreateColormap(
      this->DisplayId, XRootWindow(this->DisplayId, v->screen), v->visual, AllocNone);

    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = this->ColorMap;
    attr.event_mask = StructureNotifyMask | ExposureMask;

    // get a default parent if one has not been set.
    if (!this->ParentId)
    {
      this->ParentId = XRootWindow(this->DisplayId, v->screen);
    }
    this->WindowId =
      XCreateWindow(this->DisplayId, this->ParentId, x, y, static_cast<unsigned int>(width),
        static_cast<unsigned int>(height), 0, v->depth, InputOutput, v->visual,
        CWBackPixel | CWBorderPixel | CWColormap | CWOverrideRedirect | CWEventMask, &attr);
    XStoreName(this->DisplayId, this->WindowId, this->WindowName);
    XSetNormalHints(this->DisplayId, this->WindowId, &xsh);

    char classStr[4] = "Vtk";
    char nameStr[4] = "vtk";
    xch.res_class = classStr;
    xch.res_name = nameStr;
    XSetClassHint(this->DisplayId, this->WindowId, &xch);

    this->OwnWindow = true;
  }
  else
  {
    XChangeWindowAttributes(this->DisplayId, this->WindowId, CWOverrideRedirect, &attr);
    XGetWindowAttributes(this->DisplayId, this->WindowId, &winattr);
    matcher.visualid = XVisualIDFromVisual(winattr.visual);
    matcher.screen = XDefaultScreen(DisplayId);
    v = XGetVisualInfo(this->DisplayId, VisualIDMask | VisualScreenMask, &matcher, &nItems);
  }

  if (this->OwnWindow)
  {
    // RESIZE THE WINDOW TO THE DESIRED SIZE
    vtkDebugMacro(<< "Resizing the xwindow\n");
    XResizeWindow(this->DisplayId, this->WindowId,
      ((this->Size[0] > 0) ? static_cast<unsigned int>(this->Size[0]) : 300),
      ((this->Size[1] > 0) ? static_cast<unsigned int>(this->Size[1]) : 300));
    XSync(this->DisplayId, False);
  }

  if (this->OwnWindow && this->ShowWindow)
  {
    vtkDebugMacro(" Mapping the xwindow\n");
    XMapWindow(this->DisplayId, this->WindowId);
    XSync(this->DisplayId, False);
    XEvent e;
    XIfEvent(this->DisplayId, &e, XEventTypeEquals<MapNotify>, nullptr);
    XGetWindowAttributes(this->DisplayId, this->WindowId, &winattr);
    // if the specified window size is bigger than the screen size,
    // we have to reset the window size to the screen size
    width = winattr.width;
    height = winattr.height;
    this->Mapped = 1;

    if (this->FullScreen)
    {
      XGrabKeyboard(
        this->DisplayId, this->WindowId, False, GrabModeAsync, GrabModeAsync, CurrentTime);
    }
  }
  this->Size[0] = width;
  this->Size[1] = height;
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::Destroy()
{
  if (this->DisplayId && this->WindowId)
  {
    // we will only have a cursor defined if a CurrentCursor has been
    // set > 0 or if the cursor has been hidden... if we undefine without
    // checking, bad things can happen (BadWindow)
    if (this->GetCurrentCursor() || this->CursorHidden)
    {
      XUndefineCursor(this->DisplayId, this->WindowId);
    }
    if (this->XCArrow)
    {
      XFreeCursor(this->DisplayId, this->XCArrow);
    }
    if (this->XCCrosshair)
    {
      XFreeCursor(this->DisplayId, this->XCCrosshair);
    }
    if (this->XCSizeAll)
    {
      XFreeCursor(this->DisplayId, this->XCSizeAll);
    }
    if (this->XCSizeNS)
    {
      XFreeCursor(this->DisplayId, this->XCSizeNS);
    }
    if (this->XCSizeWE)
    {
      XFreeCursor(this->DisplayId, this->XCSizeWE);
    }
    if (this->XCSizeNE)
    {
      XFreeCursor(this->DisplayId, this->XCSizeNE);
    }
    if (this->XCSizeNW)
    {
      XFreeCursor(this->DisplayId, this->XCSizeNW);
    }
    if (this->XCSizeSE)
    {
      XFreeCursor(this->DisplayId, this->XCSizeSE);
    }
    if (this->XCSizeSW)
    {
      XFreeCursor(this->DisplayId, this->XCSizeSW);
    }
    if (this->XCHand)
    {
      XFreeCursor(this->DisplayId, this->XCHand);
    }
    if (this->XCCustom)
    {
      XFreeCursor(this->DisplayId, this->XCCustom);
    }
    if (this->OwnWindow)
    {
      XDestroyWindow(this->DisplayId, this->WindowId);
      this->WindowId = static_cast<Window>(0);
    }
    else
    {
      // if we don't own it, simply unmap the window
      XUnmapWindow(this->DisplayId, this->WindowId);
    }
    this->Mapped = 0;
  }

  this->XCCrosshair = 0;
  this->XCArrow = 0;
  this->XCSizeAll = 0;
  this->XCSizeNS = 0;
  this->XCSizeWE = 0;
  this->XCSizeNE = 0;
  this->XCSizeNW = 0;
  this->XCSizeSE = 0;
  this->XCSizeSW = 0;
  this->XCHand = 0;
  this->XCCustom = 0;

  this->CloseDisplay();

  // make sure all other code knows we're not mapped anymore
  this->Mapped = 0;
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::SetSize(int x, int y)
{
  static bool resizing = false;
  if ((this->Size[0] != x) || (this->Size[1] != y))
  {
    this->Superclass::SetSize(x, y);

    if (!this->UseOffScreenBuffers)
    {
      if (this->WindowId)
      {
        XResizeWindow(this->DisplayId, this->WindowId, static_cast<unsigned int>(x),
          static_cast<unsigned int>(y));
        XSync(this->DisplayId, False);
        XWindowAttributes attribs;
        XGetWindowAttributes(this->DisplayId, this->WindowId, &attribs);
        if (attribs.width != x || attribs.height != y)
        {
          XEvent e;
          XIfEvent(this->DisplayId, &e, XEventTypeEquals<ConfigureNotify>, nullptr);
        }
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::SetPosition(int x, int y)
{
  static bool resizing = false;

  if ((this->Position[0] != x) || (this->Position[1] != y))
  {
    this->Modified();
    this->Position[0] = x;
    this->Position[1] = y;
    if (this->WindowId)
    {
      XMoveWindow(this->DisplayId, this->WindowId, x, y);
      XSync(this->DisplayId, False);
    }
  }
}

//-------------------------------------------------------------------------------------------------
XVisualInfo* vtkXlibHardwareWindow::GetDesiredVisualInfo()
{
  static XVisualInfo visual;
  if (!this->OpenDisplay())
  {
    return nullptr;
  }
  bool haveVisual = false;
  // Accept either a TrueColor or DirectColor visual at any multiple-of-8 depth.
  for (int depth = 32; depth > 0 && !haveVisual; depth -= 8)
  {
    if (XMatchVisualInfo(this->DisplayId, /*screen*/ 0, depth, /*class*/ TrueColor, &visual))
    {
      haveVisual = true;
      break;
    }
    if (XMatchVisualInfo(this->DisplayId, /*screen*/ 0, depth, /*class*/ DirectColor, &visual))
    {
      haveVisual = true;
      break;
    }
  }
  return haveVisual ? &visual : nullptr;
}

//-------------------------------------------------------------------------------------------------
vtkTypeBool vtkXlibHardwareWindow::OpenDisplay()
{
  if (this->DisplayId == nullptr)
  {
    this->DisplayId = XOpenDisplay(static_cast<char*>(nullptr));
    if (this->DisplayId)
    {
      this->OwnDisplay = 1;
      return true;
    }
    vtkErrorMacro(<< "bad X server connection. DISPLAY=" << vtksys::SystemTools::GetEnv("DISPLAY")
                  << ". Aborting.\n");
    return false;
  }
  return true;
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::CloseDisplay()
{
  // if we create the display, we'll delete it
  if (this->OwnDisplay && this->DisplayId)
  {
    XCloseDisplay(this->DisplayId);
    this->DisplayId = nullptr;
    this->OwnDisplay = false;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::HideCursor()
{
  static char blankBits[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00 };

  static XColor black = { 0, 0, 0, 0, 0, 0 };

  if (!this->DisplayId || !this->WindowId)
  {
    this->CursorHidden = 1;
  }
  else if (!this->CursorHidden)
  {
    Pixmap blankPixmap = XCreateBitmapFromData(this->DisplayId, this->WindowId, blankBits, 16, 16);

    Cursor blankCursor =
      XCreatePixmapCursor(this->DisplayId, blankPixmap, blankPixmap, &black, &black, 7, 7);

    XDefineCursor(this->DisplayId, this->WindowId, blankCursor);

    XFreePixmap(this->DisplayId, blankPixmap);

    this->CursorHidden = 1;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::ShowCursor()
{
  if (!this->DisplayId || !this->WindowId)
  {
    this->CursorHidden = 0;
  }
  else if (this->CursorHidden)
  {
    XUndefineCursor(this->DisplayId, this->WindowId);
    this->CursorHidden = 0;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkXlibHardwareWindow::SetCurrentCursor(int shape)
{
  if (this->InvokeEvent(vtkCommand::CursorChangedEvent, &shape))
  {
    return;
  }
  this->Superclass::SetCurrentCursor(shape);
  if (!this->DisplayId || !this->WindowId)
  {
    return;
  }

  if (shape == VTK_CURSOR_DEFAULT)
  {
    XUndefineCursor(this->DisplayId, this->WindowId);
    return;
  }

  switch (shape)
  {
    case VTK_CURSOR_CROSSHAIR:
      if (!this->XCCrosshair)
      {
        this->XCCrosshair = XCreateFontCursor(this->DisplayId, XC_crosshair);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCCrosshair);
      break;
    case VTK_CURSOR_ARROW:
      if (!this->XCArrow)
      {
        this->XCArrow = XCreateFontCursor(this->DisplayId, XC_top_left_arrow);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCArrow);
      break;
    case VTK_CURSOR_SIZEALL:
      if (!this->XCSizeAll)
      {
        this->XCSizeAll = XCreateFontCursor(this->DisplayId, XC_fleur);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeAll);
      break;
    case VTK_CURSOR_SIZENS:
      if (!this->XCSizeNS)
      {
        this->XCSizeNS = XCreateFontCursor(this->DisplayId, XC_sb_v_double_arrow);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNS);
      break;
    case VTK_CURSOR_SIZEWE:
      if (!this->XCSizeWE)
      {
        this->XCSizeWE = XCreateFontCursor(this->DisplayId, XC_sb_h_double_arrow);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeWE);
      break;
    case VTK_CURSOR_SIZENE:
      if (!this->XCSizeNE)
      {
        this->XCSizeNE = XCreateFontCursor(this->DisplayId, XC_top_right_corner);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNE);
      break;
    case VTK_CURSOR_SIZENW:
      if (!this->XCSizeNW)
      {
        this->XCSizeNW = XCreateFontCursor(this->DisplayId, XC_top_left_corner);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNW);
      break;
    case VTK_CURSOR_SIZESE:
      if (!this->XCSizeSE)
      {
        this->XCSizeSE = XCreateFontCursor(this->DisplayId, XC_bottom_right_corner);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeSE);
      break;
    case VTK_CURSOR_SIZESW:
      if (!this->XCSizeSW)
      {
        this->XCSizeSW = XCreateFontCursor(this->DisplayId, XC_bottom_left_corner);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeSW);
      break;
    case VTK_CURSOR_HAND:
      if (!this->XCHand)
      {
        this->XCHand = XCreateFontCursor(this->DisplayId, XC_hand1);
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCHand);
      break;
    case VTK_CURSOR_CUSTOM:
#if VTK_HAVE_XCURSOR
      this->XCCustom = XcursorFilenameLoadCursor(this->DisplayId, this->GetCursorFileName());
      if (!this->XCCustom)
      {
        vtkErrorMacro(<< "Failed to load cursor from Xcursor file: " << this->GetCursorFileName());
        break;
      }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCCustom);
#else
    {
      static bool once = false;
      if (!once)
      {
        once = true;
        vtkWarningMacro("VTK built without Xcursor support; ignoring requests for custom cursors.");
      }
    }
#endif
      break;
  }
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
