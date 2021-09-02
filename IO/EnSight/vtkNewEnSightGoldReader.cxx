/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkNewEnSightGoldReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkNewEnSightGoldReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtksys/FStream.hxx"

#include <cctype> /* isspace */
#include <unordered_set>

namespace
{
constexpr int MAX_LINE_LENGTH = 256;

class EnSightFileStream
{
public:
  EnSightFileStream();
  ~EnSightFileStream();

  bool CheckVersion(const char* casefilename);
  bool ParseCaseFile(const char* casefilename);

private:
  bool OpenCaseFile(const char* casefilename);
  bool ReadNextLine(char result[MAX_LINE_LENGTH]);
  bool ReadLine(char result[MAX_LINE_LENGTH]);

  void ParseFormatSection();
  void ParseGeometrySection(const char* line);

  bool IsSectionHeader(const char* line);

  vtksys::ifstream* CaseStream;
  bool IsGoldFile = false;

  char* GeometryFileName;
};

//------------------------------------------------------------------------------
EnSightFileStream::EnSightFileStream()
{
  this->CaseStream = nullptr;
}

//------------------------------------------------------------------------------
EnSightFileStream::~EnSightFileStream()
{
  if (this->CaseStream)
  {
    if (this->CaseStream->is_open())
    {
      this->CaseStream->close();
    }
    delete this->CaseStream;
    this->CaseStream = nullptr;
  }
}

//------------------------------------------------------------------------------
bool EnSightFileStream::OpenCaseFile(const char* casefilename)
{
  this->CaseStream = new vtksys::ifstream(casefilename, ios::in);
  if (this->CaseStream->fail())
  {
    delete this->CaseStream;
    this->CaseStream = nullptr;
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool EnSightFileStream::CheckVersion(const char* casefilename)
{
  if (!this->OpenCaseFile(casefilename))
  {
    return false;
  }

  char result[MAX_LINE_LENGTH];
  while (this->ReadNextLine(result))
  {
    if (strncmp(result, "FORMAT", 6) == 0)
    {
      this->ParseFormatSection();
      break;
    }
  }
  return this->IsGoldFile;
}

//------------------------------------------------------------------------------
bool EnSightFileStream::IsSectionHeader(const char* line)
{
  std::unordered_set<std::string> headerSet = { "FORMAT", "GEOMETRY", "VARIABLE", "TIME", "FILE" };
  auto it = headerSet.find(line);
  return (it == headerSet.end() ? false : true);
}

//------------------------------------------------------------------------------
bool EnSightFileStream::ParseCaseFile(const char* casefilename)
{
  // has 5 sections: FORMAT, GEOMETRY, VARIABLE, TIME, FILE
  this->OpenCaseFile(casefilename);

  char result[MAX_LINE_LENGTH];
  auto continueRead = this->ReadNextLine(result);
  while (continueRead)
  {
    if (strncmp(result, "FORMAT", 6) == 0)
    {
      this->ParseFormatSection();
      if (!this->IsGoldFile)
      {
        vtkGenericWarningMacro("This reader handles only EnSight Gold files");
        return false;
      }
    }
    else if (strncmp(result, "GEOMETRY", 8) == 0)
    {
      while ((continueRead = this->ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
        this->ParseGeometrySection(result);
      }
      continue;
    }
    else if (strncmp(result, "VARIABLE", 8) == 0)
    {
      vtkGenericWarningMacro("VARIABLE section not yet supported");
      while ((continueRead = this->ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else if (strncmp(result, "TIME", 4) == 0)
    {
      vtkGenericWarningMacro("TIME section not yet supported");
      while ((continueRead = this->ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else if (strncmp(result, "FILE", 4) == 0)
    {
      vtkGenericWarningMacro("FILE section not yet supported");
      while ((continueRead = this->ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else
    {
      vtkGenericWarningMacro("ParseCaseFile: invalid line - " << result);
    }
    continueRead = this->ReadNextLine(result);
  }
  return true;
}

//------------------------------------------------------------------------------
void EnSightFileStream::ParseFormatSection()
{
  char line[MAX_LINE_LENGTH], subLine[MAX_LINE_LENGTH], subLine1[MAX_LINE_LENGTH];
  this->ReadNextLine(line);
  sscanf(line, " %*s %s %s", subLine, subLine1);
  if (strncmp(subLine, "ensight", 7) == 0 && strncmp(subLine1, "gold", 4) == 0)
  {
    this->IsGoldFile = true;
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::ParseGeometrySection(const char* line)
{
  char subLine[MAX_LINE_LENGTH];
  if (strncmp(line, "model:", 6) == 0)
  {
    int timeSet, fileSet;
    if (sscanf(line, " %*s %d %d%*[ \t]%s", &timeSet, &fileSet, subLine) == 3)
    {
      vtkGenericWarningMacro("timesets and filesets not supported yet.");
      this->GeometryFileName = subLine;
    }
    else if (sscanf(line, " %*s %d%*[ \t]%s", &timeSet, subLine) == 2)
    {
      vtkGenericWarningMacro("timesets not supported yet.");
      this->GeometryFileName = subLine;
    }
    else if (sscanf(line, " %*s %s", subLine) == 1)
    {
      this->GeometryFileName = subLine;
    }
  }
  else if (strncmp(line, "measured:", 9) == 0)
  {
    vtkGenericWarningMacro("measured files not supported yet");
  }
  else if (strncmp(line, "match:", 6) == 0)
  {
    vtkGenericWarningMacro("match files not supported yet");
  }
  else if (strncmp(line, "boundary:", 9) == 0)
  {
    vtkGenericWarningMacro("boundary files not supported yet");
  }
  else
  {
    vtkGenericWarningMacro("ParseGeometrySection: invalid line - " << line);
  }
}

//------------------------------------------------------------------------------
bool EnSightFileStream::ReadNextLine(char result[MAX_LINE_LENGTH])
{
  bool isComment = true;
  bool lineRead = true;

  while (isComment && lineRead)
  {
    lineRead = this->ReadLine(result);
    if (*result && result[0] != '#')
    {
      size_t len = strlen(result);
      unsigned int i = 0;
      while (i < len && (static_cast<unsigned int>(result[i]) <= 255) && isspace(result[i]))
      {
        ++i;
      }
      // If there was only space characters this is a comment, thus skip it
      if (i != len)
      {
        // The line was not empty, not beginning by '#' and not composed
        // of only white space, this is not a comment
        isComment = false;
      }
    }
  }
  return lineRead;
}

//------------------------------------------------------------------------------
// Internal function to read in a line up to MAX_LINE_LENGTH characters.
// Returns zero if there was an error.
bool EnSightFileStream::ReadLine(char result[MAX_LINE_LENGTH])
{
  this->CaseStream->getline(result, MAX_LINE_LENGTH);
  if (this->CaseStream->fail())
  {
    // Reset the error flag before returning. This way, we can keep working
    // if we handle the error downstream.
    this->CaseStream->clear();
    return false;
  }
  return true;
}

} // end anon namespace

struct vtkNewEnSightGoldReader::ReaderImpl
{
  EnSightFileStream FileStream;
};

vtkStandardNewMacro(vtkNewEnSightGoldReader);

//----------------------------------------------------------------------------
vtkNewEnSightGoldReader::vtkNewEnSightGoldReader()
{
  this->SetNumberOfInputPorts(0);
  this->CaseFileName = nullptr;
  this->Impl = new ReaderImpl;
}

//----------------------------------------------------------------------------
vtkNewEnSightGoldReader::~vtkNewEnSightGoldReader()
{
  delete[] this->CaseFileName;
  delete this->Impl;
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::CanReadFile(const char* casefilename)
{
  return (this->Impl->FileStream.CheckVersion(casefilename) ? 1 : 0);
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::RequestInformation(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // TODO
  // check version, error if not ensight gold
  // check whether ascii, binary c, or binary fortran
  if (!this->CaseFileName)
  {
    vtkErrorMacro("CaseFileName is null");
    return 0;
  }
  this->Impl->FileStream.ParseCaseFile(this->CaseFileName);

  return 1;
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  vtkPartitionedDataSetCollection* output =
    vtkPartitionedDataSetCollection::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  return 1;
}

//------------------------------------------------------------------------------
void vtkNewEnSightGoldReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Case FileName: " << (this->CaseFileName ? this->CaseFileName : "(none)") << "\n";
}
