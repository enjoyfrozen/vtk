/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkHierarchicalBoxDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkHierarchicalBoxDataIterator.h"

vtkStandardNewMacro(vtkHierarchicalBoxDataSet);

//-----------------------------------------------------------------------------
vtkHierarchicalBoxDataSet::vtkHierarchicalBoxDataSet() = default;

//-----------------------------------------------------------------------------
vtkHierarchicalBoxDataSet::~vtkHierarchicalBoxDataSet() = default;

//-----------------------------------------------------------------------------
void vtkHierarchicalBoxDataSet::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//----------------------------------------------------------------------------
vtkCompositeDataIterator* vtkHierarchicalBoxDataSet::NewIterator()
{
  vtkCompositeDataIterator* iter = vtkHierarchicalBoxDataIterator::New();
  iter->SetDataSet(this);
  return iter;
}

//-----------------------------------------------------------------------------
vtkHierarchicalBoxDataSet* vtkHierarchicalBoxDataSet::GetData(
  vtkInformation* info)
{
  return
    info?vtkHierarchicalBoxDataSet::SafeDownCast(info->Get(DATA_OBJECT())) : nullptr;
}

//----------------------------------------------------------------------------
vtkHierarchicalBoxDataSet* vtkHierarchicalBoxDataSet::GetData(
  vtkInformationVector* v, int i)
{
  return vtkHierarchicalBoxDataSet::GetData(v->GetInformationObject(i));
}
