/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
// QT includes
#include <QApplication>
#include <QSurfaceFormat>

#include "QVTKOpenGLNativeWidget.h"
#include "CustomLinkView.h"

extern int qInitResources_icons();

int main( int argc, char** argv )
{
  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  // QT Stuff
  QApplication app( argc, argv );

  QApplication::setStyle("fusion");

  qInitResources_icons();

  CustomLinkView myCustomLinkView;
  myCustomLinkView.show();

  return app.exec();
}
