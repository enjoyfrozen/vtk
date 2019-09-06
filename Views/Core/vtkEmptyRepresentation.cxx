/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkEmptyRepresentation.h"

#include "vtkAnnotationLink.h"
#include "vtkConvertSelectionDomain.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkEmptyRepresentation);


vtkEmptyRepresentation::vtkEmptyRepresentation()
{
  this->ConvertDomains = vtkSmartPointer<vtkConvertSelectionDomain>::New();

  this->SetNumberOfInputPorts(0);
}

vtkEmptyRepresentation::~vtkEmptyRepresentation() = default;

//----------------------------------------------------------------------------
vtkAlgorithmOutput* vtkEmptyRepresentation::GetInternalAnnotationOutputPort(
  int vtkNotUsed(port), int vtkNotUsed(conn))
{
  this->ConvertDomains->SetInputConnection(0,
    this->GetAnnotationLink()->GetOutputPort(0));
  this->ConvertDomains->SetInputConnection(1,
    this->GetAnnotationLink()->GetOutputPort(1));

  return this->ConvertDomains->GetOutputPort();
}

void vtkEmptyRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
