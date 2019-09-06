/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

// .NAME
// .SECTION Description
// this program tests the Directory class

#include "vtkDirectory.h"
#include "vtkDebugLeaks.h"

int TestDirectory(int,char *[])
{
  vtkDirectory *myDir = vtkDirectory::New();
  vtkDirectory *testDir = vtkDirectory::New();
  myDir->Open (".");
  char buf[1024];
  myDir->GetCurrentWorkingDirectory(buf, 1024);
  cout << "Working Directory: " << buf << endl;
  // Get each file name in the directory
  for (int i = 0; i < myDir->GetNumberOfFiles(); i++)
  {
    cout << (testDir->Open(myDir->GetFile(i)) == 0 ? "file: " : "dir:  ") <<  myDir->GetFile(i) << endl;
  }

  myDir->Delete();
  testDir->Delete();
  return 0;
}
