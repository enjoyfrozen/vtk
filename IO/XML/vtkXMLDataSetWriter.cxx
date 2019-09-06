/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkXMLDataSetWriter.h"

#include "vtkAlgorithmOutput.h"
#include "vtkCallbackCommand.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkStructuredGrid.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLRectilinearGridWriter.h"
#include "vtkXMLStructuredGridWriter.h"
#include "vtkXMLUnstructuredGridWriter.h"

vtkStandardNewMacro(vtkXMLDataSetWriter);

//----------------------------------------------------------------------------
vtkXMLDataSetWriter::vtkXMLDataSetWriter() = default;

//----------------------------------------------------------------------------
vtkXMLDataSetWriter::~vtkXMLDataSetWriter() = default;

//----------------------------------------------------------------------------
int vtkXMLDataSetWriter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}
