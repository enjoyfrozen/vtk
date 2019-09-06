/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkLogoWidget.h"
#include "vtkLogoRepresentation.h"
#include "vtkCallbackCommand.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkLogoWidget);

//-------------------------------------------------------------------------
vtkLogoWidget::vtkLogoWidget()
{
  this->Selectable = 0;
}

//-------------------------------------------------------------------------
vtkLogoWidget::~vtkLogoWidget() = default;

//----------------------------------------------------------------------
void vtkLogoWidget::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
  {
    this->WidgetRep = vtkLogoRepresentation::New();
  }
}

//-------------------------------------------------------------------------
void vtkLogoWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
