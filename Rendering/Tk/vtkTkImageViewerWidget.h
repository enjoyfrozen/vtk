/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTkImageViewerWidget
 * @brief   a Tk Widget for viewing vtk images
 *
 *
 * vtkTkImageViewerWidget is a Tk widget that you can render into. It has a
 * GetImageViewer method that returns a vtkImageViewer. You can also
 * specify a vtkImageViewer to be used when creating the widget by using
 * the -iv option. It also takes -width and -height options.
 * Events can be bound on this widget just like any other Tk widget.
 *
 * @sa
 * vtkImageViewer
*/

#ifndef vtkTkImageViewerWidget_h
#define vtkTkImageViewerWidget_h

#include "vtkImageViewer.h"
#include "vtkTcl.h"
#include "vtkTk.h"
#ifndef VTK_PYTHON_BUILD
#include "vtkTclUtil.h"
#endif
#include "vtkWindows.h"

struct vtkTkImageViewerWidget
{
  Tk_Window  TkWin;             /* Tk window structure */
  Tcl_Interp *Interp;           /* Tcl interpreter */
  int Width;
  int Height;
  vtkImageViewer *ImageViewer;
  char *IV;
#ifdef _WIN32
  WNDPROC OldProc;
#endif
};

#endif
// VTK-HeaderTest-Exclude: vtkTkImageViewerWidget.h
