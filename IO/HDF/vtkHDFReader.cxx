/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHDFReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkHDFReader.h"

#include "vtkArrayIteratorIncludes.h"
#include "vtkCallbackCommand.h"
#include "vtkDataArray.h"
#include "vtkDataArraySelection.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkHDFReaderImplementation.h"
#include "vtkHDFReaderVersion.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnstructuredGrid.h"

#include "vtksys/Encoding.hxx"
#include "vtksys/FStream.hxx"
#include <vtksys/SystemTools.hxx>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <functional>
#include <locale> // C++ locale
#include <sstream>
#include <vector>

vtkStandardNewMacro(vtkHDFReader);
vtkCxxSetObjectMacro(vtkHDFReader, ReaderErrorObserver, vtkCommand);

//----------------------------------------------------------------------------
vtkHDFReader::vtkHDFReader()
{
  this->FileName = nullptr;
  this->ReaderErrorObserver = nullptr;
  // Setup the selection callback to modify this object when an array
  // selection is changed.
  this->SelectionObserver = vtkCallbackCommand::New();
  this->SelectionObserver->SetCallback(&vtkHDFReader::SelectionModifiedCallback);
  this->SelectionObserver->SetClientData(this);
  for (int i = 0; i < vtkHDFReader::GetNumberOfAttributeTypes(); ++i)
  {
    this->DataArraySelection[i] = vtkDataArraySelection::New();
    this->DataArraySelection[i]->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
  }
  this->InformationError = 0;
  this->DataError = 0;
  this->ReadError = 0;
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);

  // Lower dimensional cell data support.
  this->AxesEmpty[0] = 0;
  this->AxesEmpty[1] = 0;
  this->AxesEmpty[2] = 0;
  std::fill(this->WholeExtent, this->WholeExtent + 6, 0);
  std::fill(this->Origin, this->Origin + 3, 0.0);
  std::fill(this->Spacing, this->Spacing + 3, 0.0);

  this->CurrentOutput = nullptr;
  this->Impl = new vtkHDFReader::Implementation(this);
}

//----------------------------------------------------------------------------
vtkHDFReader::~vtkHDFReader()
{
  delete this->Impl;
  this->SetFileName(nullptr);
  for (int i = 0; i < vtkHDFReader::GetNumberOfAttributeTypes(); ++i)
  {
    this->DataArraySelection[i]->RemoveObserver(this->SelectionObserver);
    this->DataArraySelection[i]->Delete();
  }
  this->SelectionObserver->Delete();
  if (this->ReaderErrorObserver)
  {
    this->ReaderErrorObserver->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkHDFReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FileName: " << (this->FileName ? this->FileName : "(none)") << "\n";
  os << indent << "CellDataArraySelection: " << this->DataArraySelection[vtkDataObject::CELL]
     << "\n";
  os << indent << "PointDataArraySelection: " << this->DataArraySelection[vtkDataObject::POINT]
     << "\n";
}

//----------------------------------------------------------------------------
vtkDataSet* vtkHDFReader::GetOutputAsDataSet()
{
  return this->GetOutputAsDataSet(0);
}

//----------------------------------------------------------------------------
vtkDataSet* vtkHDFReader::GetOutputAsDataSet(int index)
{
  return vtkDataSet::SafeDownCast(this->GetOutputDataObject(index));
}

//----------------------------------------------------------------------------
// Major version should be incremented when older readers can no longer
// read files written for this reader. Minor versions are for added
// functionality that can be safely ignored by older readers.
int vtkHDFReader::CanReadFileVersion(int major, int vtkNotUsed(minor))
{
  return (major > vtkHDFReaderMajorVersion) ? 0 : 1;
}

//----------------------------------------------------------------------------
void vtkHDFReader::SetupOutputData()
{
  // Initialize the output.
  this->CurrentOutput->Initialize();
}

//----------------------------------------------------------------------------
void vtkHDFReader::SetupEmptyOutput()
{
  this->GetCurrentOutput()->Initialize();
}

//----------------------------------------------------------------------------
int vtkHDFReader::CanReadFile(const char* name)
{
  // First make sure the file exists.  This prevents an empty file
  // from being created on older compilers.
  vtksys::SystemTools::Stat_t fs;
  if (vtksys::SystemTools::Stat(name, &fs) != 0)
  {
    vtkErrorMacro("File does not exist: " << name);
    return 0;
  }
  if (!this->Impl->Open(name))
  {
    return 0;
  }
  return 1;
}

//----------------------------------------------------------------------------
void vtkHDFReader::SelectionModifiedCallback(vtkObject*, unsigned long, void* clientdata, void*)
{
  static_cast<vtkHDFReader*>(clientdata)->Modified();
}

//----------------------------------------------------------------------------
int vtkHDFReader::GetNumberOfPointArrays()
{
  return this->DataArraySelection[vtkDataObject::POINT]->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
const char* vtkHDFReader::GetPointArrayName(int index)
{
  return this->DataArraySelection[vtkDataObject::POINT]->GetArrayName(index);
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkHDFReader::GetPointDataArraySelection()
{
  return this->DataArraySelection[vtkDataObject::POINT];
}

//----------------------------------------------------------------------------
int vtkHDFReader::GetPointArrayStatus(const char* name)
{
  return this->DataArraySelection[vtkDataObject::POINT]->ArrayIsEnabled(name);
}

//----------------------------------------------------------------------------
void vtkHDFReader::SetPointArrayStatus(const char* name, int status)
{
  if (status)
  {
    this->DataArraySelection[vtkDataObject::POINT]->EnableArray(name);
  }
  else
  {
    this->DataArraySelection[vtkDataObject::POINT]->DisableArray(name);
  }
}

//----------------------------------------------------------------------------
int vtkHDFReader::GetNumberOfCellArrays()
{
  return this->DataArraySelection[vtkDataObject::CELL]->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
vtkDataArraySelection* vtkHDFReader::GetCellDataArraySelection()
{
  return this->DataArraySelection[vtkDataObject::CELL];
}

//----------------------------------------------------------------------------
const char* vtkHDFReader::GetCellArrayName(int index)
{
  return this->DataArraySelection[vtkDataObject::CELL]->GetArrayName(index);
}

//----------------------------------------------------------------------------
int vtkHDFReader::GetCellArrayStatus(const char* name)
{
  return this->DataArraySelection[vtkDataObject::CELL]->ArrayIsEnabled(name);
}

//----------------------------------------------------------------------------
void vtkHDFReader::SetCellArrayStatus(const char* name, int status)
{
  if (status)
  {
    this->DataArraySelection[vtkDataObject::CELL]->EnableArray(name);
  }
  else
  {
    this->DataArraySelection[vtkDataObject::CELL]->DisableArray(name);
  }
}

//----------------------------------------------------------------------------
vtkDataObject* vtkHDFReader::GetCurrentOutput()
{
  return this->CurrentOutput;
}

//----------------------------------------------------------------------------
vtkInformation* vtkHDFReader::GetCurrentOutputInformation()
{
  return this->CurrentOutputInformation;
}

//------------------------------------------------------------------------------
int vtkHDFReader::RequestDataObject(vtkInformation*, vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* outputVector)
{
  std::map<int, std::string> typeNameMap = { std::make_pair(VTK_IMAGE_DATA, "vtkImageData"),
    std::make_pair(VTK_UNSTRUCTURED_GRID, "vtkUnstructuredGrid") };
  vtkInformation* info = outputVector->GetInformationObject(0);
  vtkDataSet* output = vtkDataSet::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->FileName)
  {
    vtkErrorMacro("Requires valid input file name");
    return 0;
  }

  if (!this->Impl->Open(this->FileName))
  {
    return 0;
  }
  auto version = this->Impl->GetVersion();
  if (!CanReadFileVersion(version[0], version[1]))
  {
    vtkWarningMacro("File version: " << version[0] << "." << version[1]
                                     << " is higher than "
                                        "this reader supports "
                                     << vtkHDFReaderMajorVersion << "."
                                     << vtkHDFReaderMinorVersion);
  }
  int dataSetType = this->Impl->GetDataSetType();
  if (!output || !output->IsA(typeNameMap[dataSetType].c_str()))
  {
    vtkDataSet* newOutput = nullptr;
    if (dataSetType == VTK_IMAGE_DATA)
    {
      newOutput = vtkImageData::New();
    }
    else if (dataSetType == VTK_UNSTRUCTURED_GRID)
    {
      newOutput = vtkUnstructuredGrid::New();
    }
    else
    {
      vtkErrorMacro("HDF dataset type unknown: " << dataSetType);
      return 0;
    }
    info->Set(vtkDataObject::DATA_OBJECT(), newOutput);
    newOutput->Delete();
  }
  return 1;
}

//------------------------------------------------------------------------------
int vtkHDFReader::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  if (!outInfo)
  {
    vtkErrorMacro("Invalid output information object");
    return 0;
  }
  int dataSetType = this->Impl->GetDataSetType();
  if (dataSetType == VTK_IMAGE_DATA)
  {
    if (!this->Impl->GetAttribute("WholeExtent", 6, this->WholeExtent))
    {
      return 0;
    }
    outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), this->WholeExtent, 6);
    if (!this->Impl->GetAttribute("Origin", 3, this->Origin))
    {
      return 0;
    }
    outInfo->Set(vtkDataObject::ORIGIN(), this->Origin, 3);
    if (!this->Impl->GetAttribute("Spacing", 3, this->Spacing))
    {
      return 0;
    }
    outInfo->Set(vtkDataObject::SPACING(), this->Spacing, 3);
    outInfo->Set(CAN_PRODUCE_SUB_EXTENT(), 1);
  }
  else if (dataSetType == VTK_UNSTRUCTURED_GRID)
  {
    outInfo->Set(CAN_HANDLE_PIECE_REQUEST(), 1);
  }
  else
  {
    vtkErrorMacro("Invalid dataset type: " << dataSetType);
    return 0;
  }

  for (int i = 0; i < vtkHDFReader::GetNumberOfAttributeTypes(); ++i)
  {
    this->DataArraySelection[i]->RemoveAllArrays();
    std::vector<std::string> arrayNames = this->Impl->GetArrayNames(i);
    for (std::string arrayName : arrayNames)
    {
      this->DataArraySelection[i]->AddArray(arrayName.c_str());
    }
  }
  return 1;
}

//------------------------------------------------------------------------------
int vtkHDFReader::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  if (!outInfo)
  {
    return 0;
  }
  vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());
  if (!output)
  {
    return 0;
  }
  int dataSetType = this->Impl->GetDataSetType();
  std::vector<std::string> pointArrays;
  std::vector<std::string> cellArrays;
  if (dataSetType == VTK_IMAGE_DATA)
  {
    std::array<int, 6> updateExtent;
    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), &updateExtent[0]);

    // For debugging
    int numPieces = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
    int piece = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
    int numGhosts = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());

    std::cout << "Piece:" << piece << " " << numPieces << " " << numGhosts
              << " Extent: " << updateExtent[0] << " " << updateExtent[1] << " " << updateExtent[2]
              << " " << updateExtent[3] << " " << updateExtent[4] << " " << updateExtent[5]
              << std::endl;

    vtkImageData* data = vtkImageData::SafeDownCast(output);
    data->SetOrigin(this->Origin);
    data->SetSpacing(this->Spacing);
    data->SetExtent(&updateExtent[0]);
    // in the same order as vtkDataObject::AttributeTypes: POINT, CELL
    for (int attributeType = 0; attributeType < this->GetNumberOfAttributeTypes(); ++attributeType)
    {
      std::vector<std::string> names = this->Impl->GetArrayNames(attributeType);
      for (auto name : names)
      {
        vtkDataArray* array = nullptr;
        std::array<hsize_t, 6> fileExtent;
        std::copy(updateExtent.begin(), updateExtent.end(), fileExtent.begin());
        if ((array = this->Impl->GetArray(attributeType, name.c_str(), &fileExtent[0])) == nullptr)
        {
          vtkErrorMacro("Error reading array " << name);
          return 0;
        }
        array->SetName(name.c_str());
        data->GetAttributesAsFieldData(attributeType)->AddArray(array);
        array->Delete();
      }
    }
  }
  else if (dataSetType == VTK_UNSTRUCTURED_GRID)
  {
    vtkUnstructuredGrid* data = vtkUnstructuredGrid::SafeDownCast(output);
    int memoryPiecesCount =
      outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());
    int piece = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
    int numGhosts = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());
    // in the file
    int filePiecesCount = this->Impl->GetNumberOfPieces();
    int filePiece = piece;
    while (filePiece < filePiecesCount)
    {
      // read the piece and add it to data
      filePiece += memoryPiecesCount;
    }

    std::cout << "Piece: " << piece << " NumRanks: " << numRanks << " NumGhosts: " << numGhosts
              << std::endl;
  }
  else
  {
    vtkErrorMacro("HDF dataset type unknown: " << dataSetType);
    return 0;
  }

  return 1;
}
