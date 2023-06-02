/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSDL2HardwareWindow.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkSDL2HardwareWindow.h"
#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSDL2HardwareWindow);

//-------------------------------------------------------------------------------------------------
vtkSDL2HardwareWindow::vtkSDL2HardwareWindow()
{
  this->SetWindowName(DEFAULT_BASE_WINDOW_NAME.c_str());

  // set position to -1 to let SDL place the window
  // SetPosition will still work. Defaults of 0,0 result
  // in the window title bar being off screen.
  this->Position[0] = -1;
  this->Position[1] = -1;
}

//-------------------------------------------------------------------------------------------------
vtkSDL2HardwareWindow::~vtkSDL2HardwareWindow()
{
  if (this->WindowId)
  {
    this->Destroy();
  }
}

//-------------------------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "WindowId = " << this->WindowId << endl;
}

//-------------------------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::Create()
{
  int x = ((this->Position[0] >= 0) ? this->Position[0] : SDL_WINDOWPOS_UNDEFINED);
  int y = ((this->Position[1] >= 0) ? this->Position[1] : SDL_WINDOWPOS_UNDEFINED);
  int height = ((this->Size[1] > 0) ? this->Size[1] : 300);
  int width = ((this->Size[0] > 0) ? this->Size[0] : 300);
  this->SetSize(width, height);

#ifdef __EMSCRIPTEN__
  SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
#endif

  this->WindowId = SDL_CreateWindow(this->WindowName, x, y, width, height, SDL_WINDOW_RESIZABLE);
  SDL_SetWindowResizable(this->WindowId, SDL_TRUE);
  if (this->WindowId)
  {
    int idx = SDL_GetWindowDisplayIndex(this->WindowId);
    float hdpi = 72.0;
    SDL_GetDisplayDPI(idx, nullptr, &hdpi, nullptr);
    this->SetDPI(hdpi);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::Destroy()
{
  if (this->WindowId)
  {
    SDL_DestroyWindow(this->WindowId);
    this->WindowId = nullptr;
  }
}

//------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::SetSize(int w, int h)
{
  if ((this->Size[0] != w) || (this->Size[1] != h))
  {
    this->Superclass::SetSize(w, h);
    if (this->WindowId)
    {
      int currentW, currentH;
      SDL_GetWindowSize(this->WindowId, &currentW, &currentH);
      // set the size only when window is programatically resized.
      if (currentW != w || currentH != h)
      {
        SDL_SetWindowSize(this->WindowId, w, h);
      }
    }
  }
}

//------------------------------------------------------------------------------
int* vtkSDL2HardwareWindow::GetSize()
{
  // if we aren't mapped then just return the ivar
  if (this->WindowId)
  {
    int w = 0;
    int h = 0;

    SDL_GetWindowSize(this->WindowId, &w, &h);
    this->Size[0] = w;
    this->Size[1] = h;
  }

  return this->Superclass::GetSize();
}

//------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::SetPosition(int x, int y)
{
  if ((this->Position[0] != x) || (this->Position[1] != y))
  {
    this->Modified();
    this->Position[0] = x;
    this->Position[1] = y;
    if (this->Mapped)
    {
      SDL_SetWindowPosition(this->WindowId, x, y);
    }
  }
}

//-------------------------------------------------------------------------------------------------
// Get the position in screen coordinates of the window.
int* vtkSDL2HardwareWindow::GetPosition(void)
{
  // if we aren't mapped then just return the ivar
  if (!this->Mapped)
  {
    return this->Position;
  }

  //  Find the current window position
  SDL_GetWindowPosition(this->WindowId, &this->Position[0], &this->Position[1]);

  return this->Position;
}

//-------------------------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::SetWindowName(const char* title)
{
  this->Superclass::SetWindowName(title);
  if (this->WindowId)
  {
    SDL_SetWindowTitle(this->WindowId, title);
  }
}

//-------------------------------------------------------------------------------------------------
SDL_Window* vtkSDL2HardwareWindow::GetWindowId()
{
  return this->WindowId;
}

//------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::HideCursor()
{
  SDL_ShowCursor(SDL_DISABLE);
}

//------------------------------------------------------------------------------
void vtkSDL2HardwareWindow::ShowCursor()
{
  SDL_ShowCursor(SDL_ENABLE);
}

//-------------------------------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
