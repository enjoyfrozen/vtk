/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkTextWidget.h"
#include "vtkTextRepresentation.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"

vtkStandardNewMacro(vtkTextWidget);

//-------------------------------------------------------------------------
vtkTextWidget::vtkTextWidget() = default;

//-------------------------------------------------------------------------
vtkTextWidget::~vtkTextWidget() = default;

//-------------------------------------------------------------------------
void vtkTextWidget::SetTextActor(vtkTextActor *textActor)
{
  vtkTextRepresentation *textRep = reinterpret_cast<vtkTextRepresentation*>(this->WidgetRep);
  if ( ! textRep )
  {
    this->CreateDefaultRepresentation();
    textRep = reinterpret_cast<vtkTextRepresentation*>(this->WidgetRep);
  }

  if ( textRep->GetTextActor() != textActor )
  {
    textRep->SetTextActor(textActor);
    this->Modified();
  }
}


//-------------------------------------------------------------------------
vtkTextActor *vtkTextWidget::GetTextActor()
{
  vtkTextRepresentation *textRep = reinterpret_cast<vtkTextRepresentation*>(this->WidgetRep);
  if ( ! textRep )
  {
    return nullptr;
  }
  else
  {
    return textRep->GetTextActor();
  }
}

//----------------------------------------------------------------------
void vtkTextWidget::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
  {
    this->WidgetRep = vtkTextRepresentation::New();
  }
}

//-------------------------------------------------------------------------
void vtkTextWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
