/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkXMLPUnstructuredDataWriter.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkErrorCode.h"
#include "vtkPointSet.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkXMLUnstructuredDataWriter.h"


//----------------------------------------------------------------------------
vtkXMLPUnstructuredDataWriter::vtkXMLPUnstructuredDataWriter() = default;

//----------------------------------------------------------------------------
vtkXMLPUnstructuredDataWriter::~vtkXMLPUnstructuredDataWriter() = default;

//----------------------------------------------------------------------------
void vtkXMLPUnstructuredDataWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkPointSet* vtkXMLPUnstructuredDataWriter::GetInputAsPointSet()
{
  return static_cast<vtkPointSet*>(this->GetInput());
}

//----------------------------------------------------------------------------
vtkXMLWriter* vtkXMLPUnstructuredDataWriter::CreatePieceWriter(int index)
{
  vtkXMLUnstructuredDataWriter* pWriter = this->CreateUnstructuredPieceWriter();
  pWriter->SetNumberOfPieces(this->NumberOfPieces);
  pWriter->SetWritePiece(index);
  pWriter->SetGhostLevel(this->GhostLevel);

  return pWriter;
}

//----------------------------------------------------------------------------
void vtkXMLPUnstructuredDataWriter::WritePData(vtkIndent indent)
{
  this->Superclass::WritePData(indent);
  if (this->ErrorCode == vtkErrorCode::OutOfDiskSpaceError)
  {
    return;
  }
  vtkPointSet* input = this->GetInputAsPointSet();
  this->WritePPoints(input->GetPoints(), indent);
}
