/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkProgressBarWidget.h"
#include "vtkProgressBarRepresentation.h"
#include "vtkCallbackCommand.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkProgressBarWidget);

//-------------------------------------------------------------------------
vtkProgressBarWidget::vtkProgressBarWidget()
{
  this->Selectable = 0;
}

//-------------------------------------------------------------------------
vtkProgressBarWidget::~vtkProgressBarWidget() = default;

//----------------------------------------------------------------------
void vtkProgressBarWidget::CreateDefaultRepresentation()
{
  if (!this->WidgetRep)
  {
    this->WidgetRep = vtkProgressBarRepresentation::New();
  }
}

//-------------------------------------------------------------------------
void vtkProgressBarWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
