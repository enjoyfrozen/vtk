/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
// Tests vtkQtInitialization.
// Thanks to Tim Shead from Sandia National Laboratories for writing this test.

#include "vtkQtInitialization.h"
#include "vtkSmartPointer.h"
#include "vtkTestUtilities.h"

#include <QCoreApplication>

int TestQtInitialization(int , char* [])
{
  int error_count = 0;

  // Because we share the same process with other tests, verify that
  // an instance of QCoreApplication hasn't already been created.  This
  // ensures that we don't introduce false-positives in case some other test
  // has an instance of QCoreApplication floating-around ...

  if(QCoreApplication::instance())
  {
   cerr << "Internal test error ... QCoreApplication already exists" << endl;
   ++error_count;
  }

  vtkSmartPointer<vtkQtInitialization> initialization = vtkSmartPointer<vtkQtInitialization>::New();

  if(!QCoreApplication::instance())
  {
   cerr << "QCoreApplication not initialized" << endl;
   ++error_count;
  }

  return error_count;
}
