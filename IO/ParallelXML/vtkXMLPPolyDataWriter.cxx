/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkXMLPPolyDataWriter.h"

#include "vtkObjectFactory.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkInformation.h"

vtkStandardNewMacro(vtkXMLPPolyDataWriter);

//----------------------------------------------------------------------------
vtkXMLPPolyDataWriter::vtkXMLPPolyDataWriter() = default;

//----------------------------------------------------------------------------
vtkXMLPPolyDataWriter::~vtkXMLPPolyDataWriter() = default;

//----------------------------------------------------------------------------
void vtkXMLPPolyDataWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkPolyData* vtkXMLPPolyDataWriter::GetInput()
{
  return static_cast<vtkPolyData*>(this->Superclass::GetInput());
}

//----------------------------------------------------------------------------
const char* vtkXMLPPolyDataWriter::GetDataSetName()
{
  return "PPolyData";
}

//----------------------------------------------------------------------------
const char* vtkXMLPPolyDataWriter::GetDefaultFileExtension()
{
  return "pvtp";
}

//----------------------------------------------------------------------------
vtkXMLUnstructuredDataWriter*
vtkXMLPPolyDataWriter::CreateUnstructuredPieceWriter()
{
  // Create the writer for the piece.
  vtkXMLPolyDataWriter* pWriter = vtkXMLPolyDataWriter::New();
  pWriter->SetInputConnection(this->GetInputConnection(0, 0));
  return pWriter;
}

//----------------------------------------------------------------------------
int vtkXMLPPolyDataWriter::FillInputPortInformation(
  int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  return 1;
}
