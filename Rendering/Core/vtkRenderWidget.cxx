/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkRenderWidget.h"

#include "vtkObjectFactory.h"
#include "vtkAbstractInteractionDevice.h"
#include "vtkAbstractRenderDevice.h"
#include "vtkRect.h"

vtkStandardNewMacro(vtkRenderWidget)

vtkRenderWidget::vtkRenderWidget()
  : Position(0, 0), Size(300, 300), Name("New VTK RenderWidget!!!")
{
}

vtkRenderWidget::~vtkRenderWidget() = default;

void vtkRenderWidget::SetPosition(const vtkVector2i &pos)
{
  if (this->Position != pos)
  {
    this->Position = pos;
    this->Modified();
  }
}

void vtkRenderWidget::SetSize(const vtkVector2i &size)
{
  if (this->Size != size)
  {
    this->Size = size;
    this->Modified();
  }
}

void vtkRenderWidget::SetName(const std::string &name)
{
  if (this->Name != name)
  {
    this->Name = name;
    this->Modified();
  }
}

void vtkRenderWidget::Render()
{
  assert(this->RenderDevice != nullptr);
  cout << "Render called!!!" << endl;
}

void vtkRenderWidget::MakeCurrent()
{
  assert(this->RenderDevice != nullptr);
  this->RenderDevice->MakeCurrent();
}

void vtkRenderWidget::Initialize()
{
  assert(this->RenderDevice != nullptr &&
         this->InteractionDevice != nullptr);
  this->InteractionDevice->SetRenderWidget(this);
  this->InteractionDevice->SetRenderDevice(this->RenderDevice);
  this->RenderDevice->CreateNewWindow(vtkRecti(this->Position.GetX(),
                                            this->Position.GetY(),
                                            this->Size.GetX(),
                                            this->Size.GetY()),
                                   this->Name);
  this->InteractionDevice->Initialize();
}

void vtkRenderWidget::Start()
{
  assert(this->InteractionDevice != nullptr);
  this->Initialize();
  this->InteractionDevice->Start();
}

void vtkRenderWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  // FIXME: Add methods for this...
  this->Superclass::PrintSelf(os, indent);
}
