/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkArrayData.h"
#include "vtkCommand.h"
#include "vtkExtractArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

///////////////////////////////////////////////////////////////////////////////
// vtkExtractArray

vtkStandardNewMacro(vtkExtractArray);

vtkExtractArray::vtkExtractArray() :
  Index(0)
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

vtkExtractArray::~vtkExtractArray() = default;

void vtkExtractArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Index: " << this->Index << endl;
}

int vtkExtractArray::FillInputPortInformation(int port, vtkInformation* info)
{
  switch(port)
  {
    case 0:
      info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkArrayData");
      return 1;
  }

  return 0;
}

int vtkExtractArray::RequestData(
  vtkInformation*,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkArrayData* const input = vtkArrayData::GetData(inputVector[0]);

  if(this->Index < 0 || this->Index >= input->GetNumberOfArrays())
  {
    vtkErrorMacro(<< "Array index " << this->Index << " out-of-range for vtkArrayData containing " << input->GetNumberOfArrays() << " arrays.");
    return 0;
  }

  vtkArrayData* const output = vtkArrayData::GetData(outputVector);
  output->ClearArrays();
  output->AddArray(input->GetArray(this->Index));

  return 1;
}

