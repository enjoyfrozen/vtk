/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkClientSocket.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkClientSocket);
//-----------------------------------------------------------------------------
vtkClientSocket::vtkClientSocket()
{
  this->ConnectingSide = false;
}

//-----------------------------------------------------------------------------
vtkClientSocket::~vtkClientSocket() = default;

//-----------------------------------------------------------------------------
int vtkClientSocket::ConnectToServer(const char* hostName, int port)
{
  if (this->SocketDescriptor != -1)
  {
    vtkWarningMacro("Client connection already exists. Closing it.");
    this->CloseSocket(this->SocketDescriptor);
    this->SocketDescriptor = -1;
  }

  this->SocketDescriptor = this->CreateSocket();
  if (this->SocketDescriptor == -1)
  {
    vtkErrorMacro("Failed to create socket.");
    return -1;
  }

  if (this->Connect(this->SocketDescriptor, hostName, port) == -1)
  {
    this->CloseSocket(this->SocketDescriptor);
    this->SocketDescriptor = -1;

    vtkErrorMacro("Failed to connect to server " << hostName << ":" << port);
    return -1;
  }

  this->ConnectingSide = true;
  return 0;
}

//-----------------------------------------------------------------------------
void vtkClientSocket::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ConnectingSide: " << this->ConnectingSide << endl;
}
