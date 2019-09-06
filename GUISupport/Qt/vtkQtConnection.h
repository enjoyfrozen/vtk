/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



// .SECTION Description
// vtkQtConnection is an internal class.


#ifndef vtkQtConnection_h
#define vtkQtConnection_h

#include "vtkCommand.h"  // for event defines
#include <QObject>

class vtkObject;
class vtkCallbackCommand;
class vtkEventQtSlotConnect;

// class for managing a single VTK/Qt connection
// not to be included in other projects
// only here for moc to process for vtkEventQtSlotConnect
class vtkQtConnection : public QObject
{
  Q_OBJECT

  public:

    // constructor
    vtkQtConnection(vtkEventQtSlotConnect* owner);

    // destructor, disconnect if necessary
    ~vtkQtConnection() override;

    // print function
    void PrintSelf(ostream& os, vtkIndent indent);

    // callback from VTK to emit signal
    void Execute(vtkObject* caller, unsigned long event, void* client_data);

    // set the connection
    void SetConnection(vtkObject* vtk_obj, unsigned long event,
                       const QObject* qt_obj, const char* slot,
                       void* client_data, float priority=0.0
                         ,Qt::ConnectionType type = Qt::AutoConnection);

    // check if a connection matches input parameters
    bool IsConnection(vtkObject* vtk_obj, unsigned long event,
                      const QObject* qt_obj, const char* slot,
                      void* client_data);

    static void DoCallback(vtkObject* vtk_obj, unsigned long event,
                           void* client_data, void* call_data);

  signals:
    // the qt signal for moc to take care of
    void EmitExecute(vtkObject*, unsigned long, void* client_data, void* call_data, vtkCommand*);

  protected slots:
    void deleteConnection();

  protected:

    // the connection information
    vtkObject* VTKObject;
    vtkCallbackCommand* Callback;
    const QObject* QtObject;
    void* ClientData;
    unsigned long VTKEvent;
    QString QtSlot;
    vtkEventQtSlotConnect* Owner;

  private:
    vtkQtConnection(const vtkQtConnection&);
    void operator=(const vtkQtConnection&);

};

#endif
// VTK-HeaderTest-Exclude: vtkQtConnection.h
