/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkQtInitialization.h"

#include <QApplication>

vtkStandardNewMacro(vtkQtInitialization);

vtkQtInitialization::vtkQtInitialization()
{
  this->Application = nullptr;
  if(!QApplication::instance())
  {
    int argc = 0;
    this->Application = new QApplication(argc, nullptr);
  }
}

vtkQtInitialization::~vtkQtInitialization()
{
  delete this->Application;
}

void vtkQtInitialization::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "QApplication: " << QApplication::instance() << endl;
}
