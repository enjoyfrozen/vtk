/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkTestingObjectFactory.h"
#include "vtkVersion.h"
#include "vtkTestingInteractor.h"

vtkStandardNewMacro(vtkTestingObjectFactory);

VTK_CREATE_CREATE_FUNCTION(vtkTestingInteractor);

vtkTestingObjectFactory::vtkTestingObjectFactory()
{
  this->RegisterOverride("vtkRenderWindowInteractor",
                         "vtkTestingInteractor",
                         "Overrides for testing",
                         1,
                         vtkObjectFactoryCreatevtkTestingInteractor);
}

const char* vtkTestingObjectFactory::GetVTKSourceVersion()
{
  return VTK_SOURCE_VERSION;
}

void vtkTestingObjectFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Description: " << this->GetDescription() << endl;
}

