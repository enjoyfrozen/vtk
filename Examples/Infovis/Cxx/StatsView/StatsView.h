/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#ifndef StatsView_H
#define StatsView_H

#include "vtkSmartPointer.h"    // Required for smart pointer internal ivars.

#include <QMainWindow>

// Forward Qt class declarations
class Ui_StatsView;

// Forward VTK class declarations
class vtkRowQueryToTable;
class vtkQtTableView;

class StatsView : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  StatsView();
  ~StatsView() override;

public slots:

  virtual void slotOpenSQLiteDB();

protected:

protected slots:

private:

  // Methods
  void SetupSelectionLink();


  // Members
  vtkSmartPointer<vtkRowQueryToTable>     RowQueryToTable;
  vtkSmartPointer<vtkQtTableView>         TableView1;
  vtkSmartPointer<vtkQtTableView>         TableView2;
  vtkSmartPointer<vtkQtTableView>         TableView3;
  vtkSmartPointer<vtkQtTableView>         TableView4;

  // Designer form
  Ui_StatsView *ui;
};

#endif // StatsView_H
