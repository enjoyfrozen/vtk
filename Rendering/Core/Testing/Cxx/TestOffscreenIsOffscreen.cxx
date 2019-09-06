/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkNew.h"
#include "vtkRenderWindow.h"

int TestOffscreenIsOffscreen(int, char* [])
{
  vtkNew<vtkRenderWindow> renWin;
  // This test is only run if VTK_DEFAULT_RENDER_WINDOW_OFFSCREEN is on. So the default should
  // be to use offscreen rendering
  return !renWin->GetOffScreenRendering();
}
