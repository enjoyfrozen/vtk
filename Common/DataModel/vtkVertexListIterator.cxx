/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkVertexListIterator.h"

#include "vtkDataObject.h"
#include "vtkDistributedGraphHelper.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkGraph.h"

vtkStandardNewMacro(vtkVertexListIterator);
//----------------------------------------------------------------------------
vtkVertexListIterator::vtkVertexListIterator()
{
  this->Current = 0;
  this->End = 0;
  this->Graph = nullptr;
}

//----------------------------------------------------------------------------
vtkVertexListIterator::~vtkVertexListIterator()
{
  if (this->Graph)
  {
    this->Graph->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkVertexListIterator::SetGraph(vtkGraph *graph)
{
  vtkSetObjectBodyMacro(Graph, vtkGraph, graph);
  if (this->Graph)
  {
    this->Current = 0;
    this->End = this->Graph->GetNumberOfVertices();

    // For a distributed graph, shift the iteration space to cover
    // local vertices
    vtkDistributedGraphHelper *helper
      = this->Graph->GetDistributedGraphHelper();
    if (helper)
    {
      int myRank
        = this->Graph->GetInformation()->Get(vtkDataObject::DATA_PIECE_NUMBER());
      this->Current = helper->MakeDistributedId(myRank, this->Current);
      this->End = helper->MakeDistributedId(myRank, this->End);
    }
  }
}

//----------------------------------------------------------------------------
void vtkVertexListIterator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Graph: " << (this->Graph ? "" : "(null)") << endl;
  if (this->Graph)
  {
    this->Graph->PrintSelf(os, indent.GetNextIndent());
  }
}
