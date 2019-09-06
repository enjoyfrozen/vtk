/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#ifndef SimpleView_H
#define SimpleView_H

#include "vtkSmartPointer.h"    // Required for smart pointer internal ivars.
#include <QMainWindow>

// Forward Qt class declarations
class Ui_SimpleView;

// Forward VTK class declarations
class vtkQtTableView;


class SimpleView : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  SimpleView();
  ~SimpleView() override;

public slots:

  virtual void slotOpenFile();
  virtual void slotExit();

protected:

protected slots:

private:

  vtkSmartPointer<vtkQtTableView>         TableView;

  // Designer form
  Ui_SimpleView *ui;
};

#endif // SimpleView_H
