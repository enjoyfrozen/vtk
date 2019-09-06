/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



#include "QVTKApplication.h"

#include "GUI4.h"

int main(int argc, char** argv)
{
  QVTKApplication app(argc, argv);
  GUI4 widget;

  widget.show();

  return app.exec();
}
