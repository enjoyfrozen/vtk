/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



#ifndef _GUI_h
#define _GUI_h

#include <QMainWindow>
#include "ui_GUI4.h"

class vtkRenderer;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;

class GUI4 : public QMainWindow, public Ui::GUI
{
  Q_OBJECT
public:
  GUI4();
  ~GUI4();

public slots:
  void updateCoords(vtkObject*);
  void popup(vtkObject * obj, unsigned long,
             void * client_data, void *,
             vtkCommand * command);
  void color1(QAction*);
  void color2(QAction*);

protected:
  vtkRenderer* Ren1;
  vtkRenderer* Ren2;
  vtkEventQtSlotConnect* Connections;
};

#endif // _GUI_h

