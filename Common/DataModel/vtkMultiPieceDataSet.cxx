/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#include "vtkMultiPieceDataSet.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkMultiPieceDataSet);
//----------------------------------------------------------------------------
vtkMultiPieceDataSet::vtkMultiPieceDataSet() = default;

//----------------------------------------------------------------------------
vtkMultiPieceDataSet::~vtkMultiPieceDataSet() = default;

//----------------------------------------------------------------------------
vtkMultiPieceDataSet* vtkMultiPieceDataSet::GetData(vtkInformation* info)
{
  return
    info? vtkMultiPieceDataSet::SafeDownCast(info->Get(DATA_OBJECT())) : nullptr;
}

//----------------------------------------------------------------------------
vtkMultiPieceDataSet* vtkMultiPieceDataSet::GetData(vtkInformationVector* v,
                                                    int i)
{
  return vtkMultiPieceDataSet::GetData(v->GetInformationObject(i));
}

//----------------------------------------------------------------------------
void vtkMultiPieceDataSet::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

