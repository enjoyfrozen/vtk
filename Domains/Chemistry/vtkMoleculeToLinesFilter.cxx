/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#include "vtkMoleculeToLinesFilter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkMolecule.h"
#include "vtkPointData.h"

vtkStandardNewMacro(vtkMoleculeToLinesFilter);

//----------------------------------------------------------------------------
int vtkMoleculeToLinesFilter::RequestData(vtkInformation*,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkMolecule* input = vtkMolecule::SafeDownCast(vtkDataObject::GetData(inputVector[0]));
  vtkPolyData* output = vtkPolyData::SafeDownCast(vtkDataObject::GetData(outputVector));

  vtkNew<vtkCellArray> bonds;
  // 2 point ids + 1 VTKCellType = 3 values per bonds
  bonds->Allocate(3 * input->GetNumberOfBonds());

  for (vtkIdType bondInd = 0; bondInd < input->GetNumberOfBonds(); ++bondInd)
  {
    vtkBond bond = input->GetBond(bondInd);
    vtkIdType ids[2] = { bond.GetBeginAtomId(), bond.GetEndAtomId() };
    bonds->InsertNextCell(2, ids);
  }

  output->SetPoints(input->GetAtomicPositionArray());
  output->SetLines(bonds);
  output->GetPointData()->DeepCopy(input->GetAtomData());
  output->GetCellData()->DeepCopy(input->GetBondData());

  return 1;
}
