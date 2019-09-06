/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkQtView.h"

#include <QApplication>
#include <QPixmap>
#include <QWidget>
#include "vtkObjectFactory.h"


//----------------------------------------------------------------------------
vtkQtView::vtkQtView()
{

}

//----------------------------------------------------------------------------
vtkQtView::~vtkQtView()
{

}

//----------------------------------------------------------------------------
void vtkQtView::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkQtView::ProcessQtEvents()
{
  QApplication::processEvents();
}

//----------------------------------------------------------------------------
void vtkQtView::ProcessQtEventsNoUserInput()
{
  QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

//----------------------------------------------------------------------------
bool vtkQtView::SaveImage(const char* filename)
{
  return this->GetWidget() != nullptr ? this->GetWidget()->grab().save(filename) : false;
}
