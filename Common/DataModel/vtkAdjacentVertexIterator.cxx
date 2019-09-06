/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkAdjacentVertexIterator.h"

#include "vtkObjectFactory.h"
#include "vtkGraph.h"

vtkCxxSetObjectMacro(vtkAdjacentVertexIterator, Graph, vtkGraph);
vtkStandardNewMacro(vtkAdjacentVertexIterator);
//----------------------------------------------------------------------------
vtkAdjacentVertexIterator::vtkAdjacentVertexIterator()
{
  this->Vertex = 0;
  this->Current = nullptr;
  this->End = nullptr;
  this->Graph = nullptr;
}

//----------------------------------------------------------------------------
vtkAdjacentVertexIterator::~vtkAdjacentVertexIterator()
{
  if (this->Graph)
  {
    this->Graph->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkAdjacentVertexIterator::Initialize(vtkGraph *graph, vtkIdType v)
{
  this->SetGraph(graph);
  this->Vertex = v;
  vtkIdType nedges;
  this->Graph->GetOutEdges(this->Vertex, this->Current, nedges);
  this->End = this->Current + nedges;
}

//----------------------------------------------------------------------------
void vtkAdjacentVertexIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Graph: " << (this->Graph ? "" : "(null)") << endl;
  if (this->Graph)
  {
    this->Graph->PrintSelf(os, indent.GetNextIndent());
  }
  os << indent << "Vertex: " << this->Vertex << endl;
}
