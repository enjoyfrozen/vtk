/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPExtractSelectedArraysOverTime.h"

#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkPExtractDataArraysOverTime.h"

vtkStandardNewMacro(vtkPExtractSelectedArraysOverTime);
//----------------------------------------------------------------------------
vtkPExtractSelectedArraysOverTime::vtkPExtractSelectedArraysOverTime()
{
  this->ArraysExtractor = vtkSmartPointer<vtkPExtractDataArraysOverTime>::New();
  this->SetController(vtkMultiProcessController::GetGlobalController());
}

//----------------------------------------------------------------------------
vtkPExtractSelectedArraysOverTime::~vtkPExtractSelectedArraysOverTime()
{
  this->SetController(nullptr);
}

//----------------------------------------------------------------------------
void vtkPExtractSelectedArraysOverTime::SetController(vtkMultiProcessController* controller)
{
  auto extractor = vtkPExtractDataArraysOverTime::SafeDownCast(this->ArraysExtractor);
  if (extractor && extractor->GetController() != controller)
  {
    extractor->SetController(controller);
    this->Modified();
  }
}

//----------------------------------------------------------------------------
vtkMultiProcessController* vtkPExtractSelectedArraysOverTime::GetController()
{
  auto extractor = vtkPExtractDataArraysOverTime::SafeDownCast(this->ArraysExtractor);
  return (extractor ? extractor->GetController() : nullptr);
}

//----------------------------------------------------------------------------
void vtkPExtractSelectedArraysOverTime::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Controller: " << this->GetController() << endl;
}
