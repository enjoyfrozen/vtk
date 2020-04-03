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
  this->PointDataArraySelection = vtkDataArraySelection::New();
  this->CellDataArraySelection = vtkDataArraySelection::New();
  this->InformationError = 0;
  this->DataError = 0;
  this->ReadError = 0;

  // Setup the selection callback to modify this object when an array
  // selection is changed.
  this->SelectionObserver = vtkCallbackCommand::New();
  this->SelectionObserver->SetCallback(&vtkHDFReader::SelectionModifiedCallback);
  this->SelectionObserver->SetClientData(this);
  this->PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
  this->CellDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
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
  this->CellDataArraySelection->RemoveObserver(this->SelectionObserver);
  this->PointDataArraySelection->RemoveObserver(this->SelectionObserver);
  this->SelectionObserver->Delete();
  this->CellDataArraySelection->Delete();
  this->PointDataArraySelection->Delete();
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
  os << indent << "CellDataArraySelection: " << this->CellDataArraySelection << "\n";
  os << indent << "PointDataArraySelection: " << this->PointDataArraySelection << "\n";
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
  return this->PointDataArraySelection->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
const char* vtkHDFReader::GetPointArrayName(int index)
{
  return this->PointDataArraySelection->GetArrayName(index);
}

//----------------------------------------------------------------------------
int vtkHDFReader::GetPointArrayStatus(const char* name)
{
  return this->PointDataArraySelection->ArrayIsEnabled(name);
}

//----------------------------------------------------------------------------
void vtkHDFReader::SetPointArrayStatus(const char* name, int status)
{
  if (status)
  {
    this->PointDataArraySelection->EnableArray(name);
  }
  else
  {
    this->PointDataArraySelection->DisableArray(name);
  }
}

//----------------------------------------------------------------------------
int vtkHDFReader::GetNumberOfCellArrays()
{
  return this->CellDataArraySelection->GetNumberOfArrays();
}

//----------------------------------------------------------------------------
const char* vtkHDFReader::GetCellArrayName(int index)
{
  return this->CellDataArraySelection->GetArrayName(index);
}

//----------------------------------------------------------------------------
int vtkHDFReader::GetCellArrayStatus(const char* name)
{
  return this->CellDataArraySelection->ArrayIsEnabled(name);
}

//----------------------------------------------------------------------------
void vtkHDFReader::SetCellArrayStatus(const char* name, int status)
{
  if (status)
  {
    this->CellDataArraySelection->EnableArray(name);
  }
  else
  {
    this->CellDataArraySelection->DisableArray(name);
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
    vtkImageData* imageData = vtkImageData::SafeDownCast(output);
    imageData->SetOrigin(this->Origin);
    imageData->SetSpacing(this->Spacing);
    imageData->SetExtent(this->WholeExtent);
    // in the same order as vtkDataObject::AttributeTypes: POINT, CELL
    for (int attributeType = 0; attributeType < this->Impl->GetNumberOfAttributeTypes();
         ++attributeType)
    {
      std::vector<std::string> names = this->Impl->GetArrayNames(attributeType);
      for (auto name : names)
      {
        hid_t arrayType = 0;
        hid_t dataset = 0;
        int numberOfComponents = 0;
        if ((dataset = this->Impl->GetArrayInfo(
               attributeType, name.c_str(), &arrayType, &numberOfComponents)) < 0)
        {
          return 0;
        }
        // points are one more than cells
        vtkDataArray* array = nullptr;
        if ((array = this->Impl->GetArray(dataset, arrayType, attributeType, this->WholeExtent,
               numberOfComponents)) == nullptr)
        {
          vtkErrorMacro("Error reading array " << name);
          return 0;
        }
        array->SetName(name.c_str());
        imageData->GetAttributesAsFieldData(attributeType)->AddArray(array);
        array->Delete();
        H5Dclose(dataset);
      }
    }
  }
  else if (dataSetType == VTK_UNSTRUCTURED_GRID)
  {
  }
  else
  {
    vtkErrorMacro("HDF dataset type unknown: " << dataSetType);
    return 0;
  }

  return 1;
}
