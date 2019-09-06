/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
// QT includes
#include <QApplication>
#include "StatsView.h"

extern int qInitResources_icons();

int main( int argc, char** argv )
{

  // QT Stuff
  QApplication app( argc, argv );

  QApplication::setStyle("fusion");

  qInitResources_icons();

  StatsView myStatsView;
  myStatsView.show();

  return app.exec();
}
