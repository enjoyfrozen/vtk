/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "QTestMainWindow.h"

#include "vtkRegressionTestImage.h"

QTestMainWindow::QTestMainWindow(vtkRenderWindow* renWin, int ac, char** av) :
  QMainWindow(),
  RegressionImageResult(false),
  RenderWindow(renWin),
  argc(ac),
  argv(av)
{
}

bool QTestMainWindow::regressionImageResult() const
{
  return this->RegressionImageResult;
}

void QTestMainWindow::captureImage()
{
  this->RegressionImageResult = vtkRegressionTestImage(RenderWindow);
  this->close();
}
