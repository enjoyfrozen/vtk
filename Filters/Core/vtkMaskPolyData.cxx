/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#include "vtkMaskPolyData.h"

#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

vtkStandardNewMacro(vtkMaskPolyData);

vtkMaskPolyData::vtkMaskPolyData()
{
  this->OnRatio = 11;
  this->Offset = 0;
}

// Down sample polygonal data.  Don't down sample points (that is, use the
// original points, since usually not worth it.
//
int vtkMaskPolyData::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkIdType id;
  vtkPointData *pd;
  vtkIdType numCells;
  vtkIdType *pts = nullptr;
  vtkIdType npts = 0;
  int abortExecute=0;

  // Check input / pass data through
  //
  numCells = input->GetNumberOfCells();

  if ( numCells < 1 )
  {
    vtkErrorMacro (<<"No PolyData to mask!");
    return 1;
  }

  output->Allocate(input,numCells);
  input->BuildCells();

  // Traverse topological lists and traverse
  //
  vtkIdType tenth = numCells/10 + 1;
  for (id=this->Offset; id < numCells && !abortExecute; id+=this->OnRatio)
  {
    if ( ! (id % tenth) )
    {
      this->UpdateProgress ((float)id/numCells);
      abortExecute = this->GetAbortExecute();
    }
    input->GetCellPoints(id, npts, pts);
    output->InsertNextCell(input->GetCellType(id), npts, pts);
  }

  // Update ourselves and release memory
  //
  output->SetPoints(input->GetPoints());
  pd = input->GetPointData();
  output->GetPointData()->PassData(pd);

  output->Squeeze();

  return 1;
}

void vtkMaskPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "On Ratio: " << this->OnRatio << "\n";
  os << indent << "Offset: " << this->Offset << "\n";
}
