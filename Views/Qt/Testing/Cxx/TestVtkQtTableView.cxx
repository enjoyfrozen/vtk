/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkQtTableView.h"

#include "vtkSphereSource.h"
#include "vtkDataObjectToTable.h"
#include "vtkTable.h"
#include "vtkSmartPointer.h"

#include <QWidget>
#include <QApplication>
#include <QTimer>

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

int TestVtkQtTableView(int argc, char* argv[])
{
  QApplication app(argc, argv);

  // Create a sphere and create a vtkTable from its point data (normal vectors)
  VTK_CREATE(vtkSphereSource, sphereSource);
  VTK_CREATE(vtkDataObjectToTable, tableConverter);
  tableConverter->SetInputConnection(sphereSource->GetOutputPort());
  tableConverter->SetFieldType(vtkDataObjectToTable::POINT_DATA);
  tableConverter->Update();
  vtkTable* pointTable = tableConverter->GetOutput();

  // Show the table in a vtkQtTableView with split columns on
  VTK_CREATE(vtkQtTableView, tableView);
  tableView->SetSplitMultiComponentColumns(true);
  tableView->AddRepresentationFromInput(pointTable);
  tableView->Update();
  tableView->GetWidget()->show();

  // Start the Qt event loop to run the application
  QTimer::singleShot(500, &app, SLOT(quit()));
  return app.exec();
}

