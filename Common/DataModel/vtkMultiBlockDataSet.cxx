/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#include "vtkMultiBlockDataSet.h"

#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkMultiBlockDataSet);
//----------------------------------------------------------------------------
vtkMultiBlockDataSet::vtkMultiBlockDataSet() = default;

//----------------------------------------------------------------------------
vtkMultiBlockDataSet::~vtkMultiBlockDataSet() = default;

//----------------------------------------------------------------------------
vtkMultiBlockDataSet* vtkMultiBlockDataSet::GetData(vtkInformation* info)
{
  return
    info? vtkMultiBlockDataSet::SafeDownCast(info->Get(DATA_OBJECT())) : nullptr;
}

//----------------------------------------------------------------------------
vtkMultiBlockDataSet* vtkMultiBlockDataSet::GetData(vtkInformationVector* v,
                                                    int i)
{
  return vtkMultiBlockDataSet::GetData(v->GetInformationObject(i));
}

//----------------------------------------------------------------------------
void vtkMultiBlockDataSet::SetNumberOfBlocks(unsigned int numBlocks)
{
  this->Superclass::SetNumberOfChildren(numBlocks);
}


//----------------------------------------------------------------------------
unsigned int vtkMultiBlockDataSet::GetNumberOfBlocks()
{
  return this->Superclass::GetNumberOfChildren();
}

//----------------------------------------------------------------------------
vtkDataObject* vtkMultiBlockDataSet::GetBlock(unsigned int blockno)
{
  return this->Superclass::GetChild(blockno);
}

//----------------------------------------------------------------------------
void vtkMultiBlockDataSet::SetBlock(unsigned int blockno, vtkDataObject* block)
{
  if (block && block->IsA("vtkCompositeDataSet") &&
      !block->IsA("vtkMultiBlockDataSet") &&
      !block->IsA("vtkMultiPieceDataSet") &&
      !block->IsA("vtkPartitionedDataSet"))
  {
    vtkErrorMacro(<< block->GetClassName() << " cannot be added as a block.");
    return;
  }
  this->Superclass::SetChild(blockno, block);
}

//----------------------------------------------------------------------------
void vtkMultiBlockDataSet::RemoveBlock(unsigned int blockno)
{
  this->Superclass::RemoveChild(blockno);
}

//----------------------------------------------------------------------------
void vtkMultiBlockDataSet::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

