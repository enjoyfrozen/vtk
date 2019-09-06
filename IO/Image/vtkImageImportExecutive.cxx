/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkImageImportExecutive.h"

#include "vtkInformationIntegerKey.h"
#include "vtkInformationIntegerVectorKey.h"
#include "vtkObjectFactory.h"

#include "vtkAlgorithm.h"
#include "vtkAlgorithmOutput.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkImageImport.h"

vtkStandardNewMacro(vtkImageImportExecutive);

//----------------------------------------------------------------------------
int vtkImageImportExecutive::ProcessRequest(vtkInformation* request,
                                            vtkInformationVector** inInfoVec,
                                            vtkInformationVector* outInfoVec)
{
  if(this->Algorithm && request->Has(REQUEST_INFORMATION()))
  {
    // Invoke the callback
    vtkImageImport *ii = vtkImageImport::SafeDownCast(this->Algorithm);
    ii->InvokeUpdateInformationCallbacks();
  }

  return this->Superclass::ProcessRequest(request, inInfoVec, outInfoVec);
}
