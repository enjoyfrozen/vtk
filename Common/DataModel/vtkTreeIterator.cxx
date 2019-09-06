/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkTreeIterator.h"

#include "vtkTree.h"

vtkTreeIterator::vtkTreeIterator()
{
  this->Tree = nullptr;
  this->StartVertex = -1;
  this->NextId = -1;
}

vtkTreeIterator::~vtkTreeIterator()
{
  if (this->Tree)
  {
    this->Tree->Delete();
    this->Tree = nullptr;
  }
}

void vtkTreeIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Tree: " << this->Tree << endl;
  os << indent << "StartVertex: " << this->StartVertex << endl;
  os << indent << "NextId: " << this->NextId << endl;
}

void vtkTreeIterator::SetTree(vtkTree* tree)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this
                << "): setting Tree to " << tree );
  if (this->Tree != tree)
  {
    vtkTree* temp = this->Tree;
    this->Tree = tree;
    if (this->Tree != nullptr) { this->Tree->Register(this); }
    if (temp != nullptr)
    {
      temp->UnRegister(this);
    }
    this->StartVertex = -1;
    this->Initialize();
    this->Modified();
  }
}

void vtkTreeIterator::SetStartVertex(vtkIdType vertex)
{
  if (this->StartVertex != vertex)
  {
    this->StartVertex = vertex;
    this->Initialize();
    this->Modified();
  }
}

vtkIdType vtkTreeIterator::Next()
{
  vtkIdType last = this->NextId;
  if(last != -1)
  {
    this->NextId = this->NextInternal();
  }
  return last;
}

bool vtkTreeIterator::HasNext()
{
  return this->NextId != -1;
}

void vtkTreeIterator::Restart()
{
  this->Initialize();
}
