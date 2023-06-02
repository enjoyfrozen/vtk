/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSceneGraphRenderPass.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// vtk includes
#include "vtkSceneGraphRenderPass.h"

#include "vtkObjectFactory.h"
#include "vtkRenderPassCollection.h"
#include "vtkViewNode.h"

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSceneGraphRenderPass);

//-------------------------------------------------------------------------------------------------
vtkSceneGraphRenderPass::vtkSceneGraphRenderPass()
{
  this->DelegatePasses = vtkRenderPassCollection::New();
}

//-------------------------------------------------------------------------------------------------
vtkSceneGraphRenderPass::~vtkSceneGraphRenderPass()
{
  this->DelegatePasses->Delete();
  this->DelegatePasses = nullptr;
  this->PreOperations.clear();
  this->PostOperations.clear();
}

//-------------------------------------------------------------------------------------------------
void vtkSceneGraphRenderPass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "DelegatePasses:" << endl;
  this->DelegatePasses->PrintSelf(os, indent.GetNextIndent());
  os << indent << "PreOperations:" << endl;
  for (int i : this->PreOperations)
  {
    os << indent.GetNextIndent() << i << endl;
  }
  os << indent << "PostOperations:" << endl;
  for (int i : this->PostOperations)
  {
    os << indent.GetNextIndent() << i << endl;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkSceneGraphRenderPass::AddDelegatePass(vtkRenderPass* p)
{
  this->DelegatePasses->AddItem(p);
}

//-------------------------------------------------------------------------------------------------
void vtkSceneGraphRenderPass::Render(const vtkRenderState* vtkNotUsed(state)) {}

//-------------------------------------------------------------------------------------------------
void vtkSceneGraphRenderPass::Traverse(vtkViewNode* v, vtkRenderPass* parent)
{
  if (!v)
  {
    return;
  }

  this->CurrentParent = parent;
  for (int i : this->PreOperations)
  {
    v->Traverse(i);
  }
  vtkCollectionSimpleIterator iter;
  this->DelegatePasses->InitTraversal(iter);
  vtkRenderPass* pass;
  while ((pass = this->DelegatePasses->GetNextRenderPass(iter)))
  {
    vtkSceneGraphRenderPass* scpass = vtkSceneGraphRenderPass::SafeDownCast(pass);
    if (scpass)
    {
      scpass->Traverse(v, this);
    }
  }
  for (int i : this->PostOperations)
  {
    v->Traverse(i);
  }
}

VTK_ABI_NAMESPACE_END
