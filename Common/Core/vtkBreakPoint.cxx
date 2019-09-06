/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkBreakPoint.h"

#ifndef _WIN32
#include <unistd.h> // gethostname(), sleep()
#endif

// ----------------------------------------------------------------------------
void vtkBreakPoint::Break()
{
#ifndef _WIN32
  int i=0;
  char hostname[256];
  gethostname(hostname,sizeof(hostname));
  cout << "PID " << getpid() << " on " << hostname << " ready for attach"
       << endl;
  while(i==0)
  {
    sleep(5);
  }
#endif
}
