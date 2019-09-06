/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkServerSocket
 * @brief   Encapsulate a socket that accepts connections.
 *
 *
*/

#ifndef vtkServerSocket_h
#define vtkServerSocket_h

#include "vtkCommonSystemModule.h" // For export macro
#include "vtkSocket.h"

class vtkClientSocket;
class VTKCOMMONSYSTEM_EXPORT vtkServerSocket : public vtkSocket
{
public:
  static vtkServerSocket* New();
  vtkTypeMacro(vtkServerSocket, vtkSocket);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Creates a server socket at a given port and binds to it.
   * Returns -1 on error. 0 on success.
   */
  int CreateServer(int port);

  /**
   * Waits for a connection. When a connection is received
   * a new vtkClientSocket object is created and returned.
   * Returns nullptr on timeout.
   */
  vtkClientSocket* WaitForConnection(unsigned long msec=0);

  /**
   * Returns the port on which the server is running.
   */
  int GetServerPort();

protected:
  vtkServerSocket();
  ~vtkServerSocket() override;

private:
  vtkServerSocket(const vtkServerSocket&) = delete;
  void operator=(const vtkServerSocket&) = delete;
};


#endif

