/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTkRenderWidget
 * @brief   a Tk Widget for vtk rendering
 *
 *
 * vtkTkRenderWidget is a Tk widget that you can render into. It has a
 * GetRenderWindow method that returns a vtkRenderWindow. This can then
 * be used to create a vtkRenderer and etc. You can also specify a
 * vtkRenderWindow to be used when creating the widget by using
 * the -rw option. It also takes -width and -height options.
 * Events can be bound on this widget just like any other Tk widget.
 *
 * @sa
 * vtkRenderWindow vtkRenderer
*/

#ifndef vtkTkRenderWidget_h
#define vtkTkRenderWidget_h

#include "vtkRenderWindow.h"
#include "vtkTcl.h"
#include "vtkWindows.h"

// For the moment, we are not compatible w/Photo compositing
// By defining USE_COMPOSITELESS_PHOTO_PUT_BLOCK, we use the compatible
// call.
#define USE_COMPOSITELESS_PHOTO_PUT_BLOCK
#include "vtkTk.h"

#ifndef VTK_PYTHON_BUILD
#include "vtkTclUtil.h"
#endif

struct vtkTkRenderWidget
{
  Tk_Window  TkWin;             /* Tk window structure */
  Tcl_Interp *Interp;           /* Tcl interpreter */
  int Width;
  int Height;
  vtkRenderWindow *RenderWindow;
  char *RW;
#ifdef _WIN32
  WNDPROC OldProc;
#endif
};

#endif
// VTK-HeaderTest-Exclude: vtkTkRenderWidget.h
