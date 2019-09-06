/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



#include "vtkEventQtSlotConnect.h"
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkQtConnection.h"

#include <vector>

#include <qobject.h>

// hold all the connections
class vtkQtConnections : public std::vector< vtkQtConnection* > {};

vtkStandardNewMacro(vtkEventQtSlotConnect)

// constructor
vtkEventQtSlotConnect::vtkEventQtSlotConnect()
{
  Connections = new vtkQtConnections;
}


vtkEventQtSlotConnect::~vtkEventQtSlotConnect()
{
  // clean out connections
  vtkQtConnections::iterator iter;
  for(iter=Connections->begin(); iter!=Connections->end(); ++iter)
  {
    delete (*iter);
  }

  delete Connections;
}

void vtkEventQtSlotConnect::Connect(
  vtkObject* vtk_obj, unsigned long event,
  const QObject* qt_obj, const char* slot,
  void* client_data, float priority
  , Qt::ConnectionType type)
{
  if (!vtk_obj || !qt_obj)
  {
    vtkErrorMacro("Cannot connect null objects.");
    return;
  }
  vtkQtConnection* connection = new vtkQtConnection(this);
  connection->SetConnection(
    vtk_obj, event, qt_obj, slot, client_data, priority
    , type);
  Connections->push_back(connection);
}


void vtkEventQtSlotConnect::Disconnect(vtkObject* vtk_obj, unsigned long event,
                 const QObject* qt_obj, const char* slot, void* client_data)
{
  if (!vtk_obj)
  {
    vtkQtConnections::iterator iter;
    for(iter=this->Connections->begin(); iter!=this->Connections->end(); ++iter)
    {
      delete (*iter);
    }
    this->Connections->clear();
    return;
  }
  bool all_info = true;
  if(slot == nullptr || qt_obj == nullptr || event == vtkCommand::NoEvent)
  {
    all_info = false;
  }

  vtkQtConnections::iterator iter;
  for(iter=Connections->begin(); iter!=Connections->end();)
  {
    // if information matches, remove the connection
    if((*iter)->IsConnection(vtk_obj, event, qt_obj, slot, client_data))
    {
      delete (*iter);
      iter = Connections->erase(iter);
      // if user passed in all information, only remove one connection and quit
      if(all_info)
      {
        iter = Connections->end();
      }
    }
    else
    {
      ++iter;
    }
  }
}

void vtkEventQtSlotConnect::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if(Connections->empty())
  {
    os << indent << "No Connections\n";
  }
  else
  {
    os << indent << "Connections:\n";
    vtkQtConnections::iterator iter;
    for(iter=Connections->begin(); iter!=Connections->end(); ++iter)
    {
      (*iter)->PrintSelf(os, indent.GetNextIndent());
    }
  }
}

void vtkEventQtSlotConnect::RemoveConnection(vtkQtConnection* conn)
{
  vtkQtConnections::iterator iter;
  for(iter=this->Connections->begin(); iter!=this->Connections->end(); ++iter)
  {
    if(conn == *iter)
    {
      delete (*iter);
      Connections->erase(iter);
      return;
    }
  }
}

int vtkEventQtSlotConnect::GetNumberOfConnections() const
{
  return static_cast<int>(this->Connections->size());
}

