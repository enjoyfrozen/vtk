/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

// .NAME CustomLinkView - Shows custom way of linking multiple views.
//
// .SECTION Description
// CustomLinkView shows an alternate way to link various views using
// vtkEventQtSlotConnect where selection in a particular view sets
// the same selection in all other views associated.

// Other way to get the same functionality is by using vtkAnnotationLink
// shared between multiple views.

// .SECTION See Also
// EasyView

#ifndef CustomLinkView_H
#define CustomLinkView_H

#include "vtkSmartPointer.h"    // Required for smart pointer internal ivars.

#include <QMainWindow>

// Forward Qt class declarations
class Ui_CustomLinkView;

// Forward VTK class declarations
class vtkCommand;
class vtkEventQtSlotConnect;
class vtkGraphLayoutView;
class vtkObject;
class vtkQtTableView;
class vtkQtTreeView;
class vtkXMLTreeReader;

class CustomLinkView : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  CustomLinkView();
  ~CustomLinkView() override;

public slots:

  virtual void slotOpenXMLFile();
  virtual void slotExit();

protected:

protected slots:

public slots:
  // Qt signal (produced by vtkEventQtSlotConnect) will be connected to
  // this slot.
  // Full signature of the slot could be:
  // MySlot(vtkObject* caller, unsigned long vtk_event,
 //         void* clientData, void* callData, vtkCommand*)
  void selectionChanged(vtkObject*, unsigned long, void*, void* callData);

private:

  // Methods
  void SetupCustomLink();


  // Members
  vtkSmartPointer<vtkXMLTreeReader>       XMLReader;
  vtkSmartPointer<vtkGraphLayoutView>     GraphView;
  vtkSmartPointer<vtkQtTreeView>          TreeView;
  vtkSmartPointer<vtkQtTableView>         TableView;
  vtkSmartPointer<vtkQtTreeView>          ColumnView;

  // This class converts a vtkEvent to QT signal.
  vtkSmartPointer<vtkEventQtSlotConnect>  Connections;

  // Designer form
  Ui_CustomLinkView *ui;
};

#endif // CustomLinkView_H
